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
    // static int indentation = 0;
    while(peek(L) == ' ' || peek(L) == '\t'){next(L);};
    char c = peek(L);
    if(c == '\n'){
        Token t = {.type = TOKEN_EOL};
        next(L);
        return t;
    } else if(c == '\0'){
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
            while(peek(L) != ' ' && peek(L) != '\n' && peek(L) != '\0'){
                next(L);
                length++;
            }
            Token err;
            err.type = TOKEN_ERROR;
            char text[length+1];
            strncpy(text, start_pos, length);
            text[length] = '\0'; 
            printf("DEBUG: %s\n", text);
            strncpy(err.name, text, TOKEN_TEXT_MAX - 1);
            return err;

        }
        char text[length+1];
        strncpy(text, start_pos, length);
        text[length] = '\0';
        
        Token t;
        t.type = isFloat ? TOKEN_FLOAT : TOKEN_INT;
        strncpy(t.name, text, TOKEN_TEXT_MAX - 1);
        t.name[TOKEN_TEXT_MAX - 1] = '\0';
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
        t.type = TOKEN_IDENT;
        strncpy(t.name, text, TOKEN_TEXT_MAX - 1);
        t.name[TOKEN_TEXT_MAX - 1] = '\0';
        return t;
    }
    
    switch (c){
        case '+': { next(L); return (Token){.type=TOKEN_PLUS,  .val={0}};}
        case '-': { next(L); return (Token){.type=TOKEN_MINUS, .val={0}};}
        case '*': { next(L); return (Token){.type=TOKEN_MULT,  .val={0}};}
        case '%': { next(L); return (Token){.type=TOKEN_MODULO,  .val={0}};}
        case '(': { next(L); return (Token){.type=TOKEN_LPARENT, .val={0}};}
        case ')': { next(L); return (Token){.type=TOKEN_RPARENT, .val={0}};}
        case ':': { next(L); return (Token){.type=TOKEN_COLON,   .val={0}};}
        case '!': {
            next(L);
            if(peek(L) == '=') {
                next(L);
                return (Token){.type=TOKEN_NEQ, .val={0}};
            }
            printf("Unexpected token\n");
            exit(1);
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
            printf("Unexpected token\n");
            exit(1);
    }
}
