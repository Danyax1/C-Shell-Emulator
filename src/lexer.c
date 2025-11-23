#include "lexer.h"


static inline char peek(Lexer *L){ return L->charToRead[L->pos]; };
static inline char next(Lexer *L) { return L->charToRead[L->pos++]; }

int recognize_keyword(Lexer* L, char* str)
{
    char c = str[0];
    switch (c)
    {
    case 'p':
        if(strncmp(str, "print", 5) == 0 && (str[5] == ' ' || str[5] == '(')){
            L->pos += 5;
            return TOKEN_PRINT;
        }
        break;
    case 'w':
        if(strncmp(str, "while", 5) == 0 && (str[5] == ' ' || str[5] == '(')){
            L->pos += 5;
            return TOKEN_WHILE;
        }
        break;
    case 'i':
        if(strncmp(str, "if", 2) == 0 && (str[2] == ' ' || str[2] == '(')){
            L->pos += 2;
            return TOKEN_IF;
        }
        break;
    case 'e':
        char c_3 = str[2];  // elif or else - distinguish at 3rd letter
        if(c_3 == 'i'){
            if(strncmp(str, "elif", 4) == 0 && (str[4] == ' ' || str[4] == '(')){
                L->pos += 4;
                return TOKEN_ELIF;
            }
        }else{
            if(strncmp(str, "else", 4) == 0 && (str[4] == ' ' || str[4] == '(' || str[4] == ':')){
                L->pos += 4;
                return TOKEN_ELSE;
            }
        }
        break;
    case 'n':
        if(strncmp(str, "not", 3) == 0 && (str[3] == ' ' || str[3] == '(')){
            L->pos += 3;
            return TOKEN_NOT;
        }
        break;
    case 'a':
        if(strncmp(str, "and", 3) == 0 && (str[3] == ' ' || str[3] == '(')){
            L->pos += 3;
            return TOKEN_AND;
        }
        break;
    case 'o':
        if(strncmp(str, "or", 2) == 0 && (str[2] == ' ' || str[2] == '(')){
            L->pos += 2;
            return TOKEN_OR;
        }
        break;
    case 'T':
        if(strncmp(str, "True", 4) == 0 && !isalnum(str[4])){
            L->pos += 4;
            return TOKEN_TRUE;
        }
        break;
    case 'F':
        if(strncmp(str, "False", 5) == 0 && !isalnum(str[5])){
            L->pos += 5;
            return TOKEN_FALSE;
        }
        break;
    
    default:
        return -1;
    }
    return -1;
}

Token next_token(Lexer *L){
    // error recovery
    static bool last_was_error = false;
    if(last_was_error){
        last_was_error = false;
        while(peek(L) == ' ' || peek(L) == '\t') next(L);
    }
    // is indent check is needed
    // by default is enabled, but if indent error occured -> dissabled in order not to generate too many errors
    static bool is_indent_enable = true;
    if(!is_indent_enable){
        goto skip_spaces_default;
    }

    static int indentation = 0;     // should always be divisible by 4, space = 1, tab = 4
    static bool is_indent_needed = false;
    static bool is_new_line = false;

    // because we only return one token at the time, 
    // and we might need to return several dedent tokens
    // we keep track of dedent tokens needed to generate
    static int dedent_needed = 0;

    

    if(!is_new_line){
        while(peek(L) == ' ' || peek(L) == '\t'){next(L);};
    }else if(is_new_line && !is_indent_needed){
        // in that case only decreecing or remaining indent is possible
        // if indent decreeced -> generate DEDENT
        // also handle INCONSISTENT_INDENT and UNEXPECTED_INDENT
        is_new_line = false;
        int space_count = 0;
        while(peek(L) == ' ' || peek(L) == '\t'){
            if(peek(L) == ' ')  space_count++;
            if(peek(L) == '\t') space_count += 4;
            next(L);
        }
        if(space_count > indentation){
            is_indent_enable = false;
            dedent_needed = 0;
            while(peek(L) != '\n' && peek(L) != '\0'){
                next(L);
            }

            last_was_error = true;
            return (Token){.type=TOKEN_ERROR, .val.IntVal=UNEXPECTED_INDENT};
        }
        if(space_count % 4){
            is_indent_enable = false;
            dedent_needed = 0;
            while(peek(L) != '\n' && peek(L) != '\0'){
                next(L);
            }

            last_was_error = true;
            return (Token){.type=TOKEN_ERROR, .val.IntVal=INCONSISTENT_INDENT};
        }
        // if everything is correct -> count how many dedent tokens are needed and update indend count
        // skip extra new lines
        // if(peek(L) == '\n') {next(L);}
        dedent_needed = (indentation - space_count) / 4;
    }else if(is_new_line && is_indent_needed){
        is_new_line = false;
        is_indent_needed = false;
        int space_count = 0;
        while(peek(L) == ' ' || peek(L) == '\t'){
            if(peek(L) == ' ')  space_count++;
            if(peek(L) == '\t') space_count += 4;
            next(L);
        }
        // check for errors
        if(space_count <= indentation){
            is_indent_enable = false;
            dedent_needed = 0;
            while(peek(L) != '\n' && peek(L) != '\0'){
                next(L);
            }

            last_was_error = true;
            return (Token){.type=TOKEN_ERROR, .val.IntVal=EXPECTED_INDENT};
        }
        if(space_count - indentation != 4 && peek(L) != '\n'){
            is_indent_enable = false;
            dedent_needed = 0;
            while(peek(L) != '\n' && peek(L) != '\0'){
                next(L);
            }

            last_was_error = true;
            return (Token){.type=TOKEN_ERROR, .val.IntVal=INCONSISTENT_INDENT};
        }
        // skip extra new lines
        // if(peek(L) == '\n') {next(L);}
        indentation = space_count;
        return (Token){.type=TOKEN_INDENT};
    }

    // dedent generator
    if(dedent_needed){
        --dedent_needed;
        indentation -= 4;
        return (Token){.type=TOKEN_DEDENT};
    }

skip_spaces_default:
    while(peek(L) == ' ' || peek(L) == '\t'){next(L);}


    char c = peek(L);
    if(c == '\n'){
        Token t = {.type = TOKEN_EOL};
        is_new_line = true;
        next(L);
        return t;
    } else if(c == '\0'){
        // handle indentations
        if(indentation){
            indentation -= 4;
            return (Token){.type=TOKEN_DEDENT};
        }
        // reset static variables 
        is_indent_enable = true;
        is_indent_needed = false;
        is_new_line = false;
        indentation = 0;
        dedent_needed = 0;

        Token t = {.type = TOKEN_EOF};
        return t;
    }
    if(isdigit(c)){
        bool isFloat = false;
        char* start_pos = L->charToRead + L->pos;
        unsigned int length = 0;
        while(isdigit(peek(L)) || (peek(L) == '.')){
            if(peek(L) == '.'){
                isFloat = true;
            }
            next(L);
            length++;
        }
        if(isalpha(peek(L))){
            // cannot be like 123abc
            // should return TOKEN_ERROR marked to the first space, eof or eol

            // error recovery
            while(peek(L) != ' ' && peek(L) != '\n' && peek(L) != '\0'){
                next(L);
                length++;
            }
            Token err;
            err.type = TOKEN_ERROR;
            err.val.IntVal = INVALID_TOKEN;
            char text[length+1];
            strncpy(text, start_pos, length);
            text[length] = '\0'; 
            strncpy(err.name, text, length+1);

            while(peek(L) != '\n' && peek(L) != '\0'){
                next(L);
            }

            last_was_error = true;
            return err;

        }
        char text[length+1];
        strncpy(text, start_pos, length);
        text[length] = '\0';
        
        Token t;
        t.type = isFloat ? TOKEN_FLOAT : TOKEN_INT;
        strncpy(t.name, text, length+1);
        if (isFloat) {
            t.val.FloatVal = atof(text);
        }else {
            t.val.IntVal = atoi(text);    
        }
        return t;
    }
    if(isalpha(c) || c == '_'){
        char* start_pos = L->charToRead + L->pos;
        int key_w = recognize_keyword(L, start_pos);
        if(key_w > 0){
            Token t;
            t.type = key_w;
            if(key_w == TOKEN_TRUE) t.val.IntVal = 1;
            if(key_w == TOKEN_FALSE) t.val.IntVal = 0;
            return t;
        }
        unsigned int length = 0;
        while(isalnum(peek(L)) || (peek(L) == '_')){
            next(L);
            length++;
        }
        char text[length+1];
        strncpy(text, start_pos, length);
        text[length] = '\0';
        Token t;
        t.type = TOKEN_ID;
        strncpy(t.name, text, length+1);
        return t;
    }
    
    switch (c){
        case '+': { next(L); return (Token){.type=TOKEN_PLUS,  .val={0}};}
        case '-': { next(L); return (Token){.type=TOKEN_MINUS, .val={0}};}
        case '*': { next(L); return (Token){.type=TOKEN_MULT,  .val={0}};}
        case '%': { next(L); return (Token){.type=TOKEN_MODULO,  .val={0}};}
        case '(': { next(L); return (Token){.type=TOKEN_LPARENT, .val={0}};}
        case ')': { next(L); return (Token){.type=TOKEN_RPARENT, .val={0}};}
        case ':': {
            next(L);
            is_indent_needed = true;
            return (Token){.type=TOKEN_COLON};
        }
        case '!': {
            next(L);
            if(peek(L) == '=') {
                next(L);
                return (Token){.type=TOKEN_NEQ, .val={0}};
            }
            int len = 0;
            char* start_pos = L->charToRead + L->pos;
            while(peek(L) != ' ' && peek(L) != '\n' && peek(L) != '\0'){
                next(L);
                len++;
            }
            Token err;
            err.type = TOKEN_ERROR;
            err.val.IntVal = INVALID_TOKEN;
            char text[len+1];
            strncpy(text, start_pos, len);
            text[len] = '\0'; 
            strncpy(err.name, text, len+1);
            while(peek(L) != '\n' && peek(L) != '\0'){
                next(L);
            }

            last_was_error = true;
            return err;
        }
        case '/': {
            next(L);
            if(peek(L) == '/'){
                next(L);
                return (Token){.type=TOKEN_DIV_INT, .val={0}};
            }
            return (Token){.type=TOKEN_DIV, .val={0}};
        }
        case '=': {
            next(L);
            if(peek(L) == '='){
                next(L);
                return (Token){.type=TOKEN_EQ, .val={0}};
            }
            return (Token){.type=TOKEN_ASSIGN, .val={0}};
        };
        case '>': {
            next(L);
            if(peek(L) == '='){
                next(L);
                return (Token){.type=TOKEN_GE, .val={0}};
            }
            return (Token){.type=TOKEN_GT, .val={0}};
        };
        case '<': {
            next(L);
            if(peek(L) == '='){
                next(L);
                return (Token){.type=TOKEN_LE, .val={0}};
            }
            return (Token){.type=TOKEN_LT, .val={0}};
        };
        default:
            next(L);
            char buf[2] = {0};
            buf[0] = peek(L);
            Token err;
            err.type = TOKEN_ERROR;
            err.val.IntVal = UNKNOWN_SYMBOL;
            strncpy(err.name, buf, 2);
            return err;
    }
}
