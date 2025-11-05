#include "lexer.h"


static inline char peek(Lexer *L){ return L->charToRead[L->pos]; };
static inline char next(Lexer *L) { return L->charToRead[L->pos++]; }

bool recognize_keyword(char* str, char* target, int n)
{
    if(strncmp(str, target, n) == 0){
        char end = str[n];
        if(end == '\0' || end == '\n' || end == '\t' || end == ' '){
            return true;
        }
    }
    return false;
}

Token next_token(Lexer *L){
    while(isspace(peek(L))){next(L);};
    char c = peek(L);
    if(c == '\0' || c == '\n'){
        Token t = {.type = TOKEN_EOL};
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
            // cannot be like 124b
            printf("TypeError: <make a meeningfull message later>\n");
            exit(1);
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
        if(recognize_keyword(start_pos, "not", 3)){
            L->pos +=3;
            Token t;
            t.type = TOKEN_NOT;
            strncpy(t.name, "not", TOKEN_TEXT_MAX - 1);
            t.name[TOKEN_TEXT_MAX - 1] = '\0';
            return t;
        }
        if(recognize_keyword(start_pos, "and", 3)){
            L->pos +=3;
            Token t;
            t.type = TOKEN_AND;
            strncpy(t.name, "and", TOKEN_TEXT_MAX - 1);
            t.name[TOKEN_TEXT_MAX - 1] = '\0';
            return t;
        }
        if(recognize_keyword(start_pos, "or", 2)){
            L->pos +=2;
            Token t;
            t.type = TOKEN_OR;
            strncpy(t.name, "or", TOKEN_TEXT_MAX - 1);
            t.name[TOKEN_TEXT_MAX - 1] = '\0';
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
    next(L);
    switch (c) {
        case '+': return (Token){.type=TOKEN_PLUS, .name="+", .val={0}};
        case '-': return (Token){.type=TOKEN_MINUS, .name="-", .val={0}};
        case '*': return (Token){.type=TOKEN_MULT, .name="*", .val={0}};
        case '/': {
            if(peek(L) == '/'){
                next(L);
                return (Token){.type=TOKEN_DIV_INT, .name="//", .val={0}};
            }
            return (Token){.type=TOKEN_DIV, .name="/", .val={0}};
        }
        case '%': return (Token){.type=TOKEN_MODULO, .name="%", .val={0}};
        case '=': return (Token){.type=TOKEN_ASSIGN, .name="=", .val={0}};
        case '(': return (Token){.type=TOKEN_LPARENT, .name="(", .val={0}};
        case ')': return (Token){.type=TOKEN_RPARENT, .name=")", .val={0}};
        default:
            printf("Unexpected token\n");
            exit(1);
}
}
