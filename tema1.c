/*DUMINICA Ana-Maria - 312CC*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Coadanod { //structura pentru nodul din coada
    char operatie[20];
    struct Coadanod* next;
} Coadanod;

typedef struct Coada { //structura pentru coada
    Coadanod* front;
    Coadanod* rear;
} Coada;

typedef struct Nod { //structura pentru nodul din lista
    char x;
    struct Nod *next;
    struct Nod *prev;
} Nod;

typedef struct Banda { //structura banda
    Nod *banda;
    Nod *finger;
} Banda; 

typedef struct Stivanod { //structura pentru nodul din stiva
    Nod *curent;
    struct Stivanod* next;
} Stivanod;

typedef struct Stiva_Undo { //structura pentru stiva de undo
    Stivanod *top;
} Stiva_undo;

typedef struct Stiva_Redo { //structura pentru stiva de redo
    Stivanod *top;
} Stiva_redo;

void pushUndo (Stiva_undo* s1, Nod* top) { //adaug un nou pointer in stiva de undo
    Stivanod* nod_nou = (Stivanod*)malloc(sizeof(Stivanod)); //creez un nod nou
    nod_nou->curent = top; //nodul devine varful stivei
    nod_nou->next = s1->top;
    s1->top = nod_nou;
}

void pushRedo (Stiva_redo* s2, Nod* top) { //adaug un nou pointer in stiva de redo
    Stivanod* nod_nou = (Stivanod*)malloc(sizeof(Stivanod)); //creez un nod nou
    nod_nou->curent = top; //nodul devine varful stivei
    nod_nou->next = s2->top;
    s2->top = nod_nou;
}

void popUndo (Stiva_undo* s1) { //elimin un element din stiva de undo
    Stivanod *temp;
    temp = s1->top;
    s1->top = temp->next;
    free(temp);
}

void popRedo (Stiva_redo* s2) { //elimin un element din stiva de redo
    Stivanod *temp;
    temp = s2->top;
    s2->top = temp->next;
    free(temp);
}

void undo (Stiva_undo* s1, Banda* banda, Stiva_redo* s2) { 
    pushRedo(s2, banda->finger); //adaug pointerul curent in stiva de redo
    banda->finger = s1->top->curent; //modific degetul din banda
    popUndo(s1); //elimin pointerul
}

void redo (Stiva_redo* s2, Banda* banda, Stiva_undo* s1) {
    pushUndo(s1, banda->finger); //adaug pointerul curent in stiva de undo
    banda->finger = s2->top->curent; //modific degetul din banda
    popRedo(s2); //elimin pointerul
}

void enqueue (Coada* q, char ope[]) { //adaug un element in coada
    Coadanod* nod_nou = (Coadanod*)malloc(sizeof(Coadanod)); //creez un nod nou
    strcpy(nod_nou->operatie, ope); //adaug operatia in el
    nod_nou->next = NULL;

    if (q->front == NULL) { //daca coada este goala
        q->front = nod_nou;
        q->rear = nod_nou;
    }
    else {
        q->rear->next = nod_nou;
        q->rear = nod_nou;
    }
}

void dequeue (Coada* q, Banda* banda, Nod* sentinel, FILE *f, int *a, char op[]) {

    Coadanod* temp = q->front;  //elimin nodul din coada
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    if (strcmp(temp->operatie, "MOVE_RIGHT") == 0) { //daca elementul scos este MOVE_RIGHT
        if (banda->finger->next == NULL) { //daca am ajuns la capatul listei
        Nod* nod_nou = (Nod*)malloc(sizeof(Nod)); //creez un nod nou
        nod_nou->x = '#'; //adaug #
        nod_nou->prev = banda->finger;
        nod_nou->next = NULL;
        banda->finger->next = nod_nou;
        banda->finger = banda->finger->next;
        }
        else { //daca nu sunt la capatul listei
            banda->finger = banda->finger->next;  //mut pozitia degetului in dreapta
        }
        *a = 1;
        strcpy(op, temp->operatie);
    }
    if (strcmp(temp->operatie, "MOVE_LEFT") == 0) {
        if (banda->finger->prev != sentinel) { //daca nu suntem la capatul listei in stanga
            banda->finger = banda->finger->prev;
            *a = 1;
            strcpy(op, temp->operatie);
        }
        else {  //daca ajungem in capatul din stanga
            return; 
        }
    }
    if (strncmp(temp->operatie, "MOVE_RIGHT_CHAR ", 16) == 0) {
        char c;
        int len = strlen(temp->operatie);
        c = temp->operatie[len - 1];  //extrag ultimul caracter din operatie
        Nod* tempa = banda->finger;
        while (tempa->x != c && tempa->next != NULL) {  //caut caracterul in dreapta pana in capat
            tempa = tempa->next;
        }

        if (tempa->x == c) {  //daca l-am gasit mut degetul acolo
            banda->finger = tempa;
            return;
        }
        Nod* nod_nou = malloc(sizeof(Nod));  //daca nu l-am gasit atunci creez un nod nou la final
        nod_nou->x = '#';
        nod_nou->next = NULL;
        nod_nou->prev = tempa;
        tempa->next = nod_nou;
        banda->finger = nod_nou;
    }
    if (strncmp(temp->operatie, "MOVE_LEFT_CHAR ", 15) == 0) {
        char c;
        int len = strlen(temp->operatie);
        c = temp->operatie[len - 1];  //extrag ultimul caracter 
        Nod* tempa = banda->finger;
        while (tempa->x != c && tempa->prev != sentinel) {  //il caut in stanga
            tempa = tempa->prev;
        }
        if (tempa->x == c) {   //daca l-am gasit mut cursorul acolo
            banda->finger = tempa;
            return;
        }
        else {  //altfel printez eroare
            fprintf(f, "ERROR\n");
        }
    }
    if (strncmp(temp->operatie, "WRITE ", 6) == 0) {
        char c;
        int len = strlen(temp->operatie);
        c = temp->operatie[len - 1];  //extrag caracterul
        banda->finger->x = c;  //il pun in nodul curent
        *a = 1;
        strcpy(op, temp->operatie);
    }
    if (strncmp(temp->operatie, "INSERT_RIGHT ", 13) == 0) {
        char c;
        int len = strlen(temp->operatie);
        c = temp->operatie[len - 1];   //extrag caracterul
        Nod* nod_nou = (Nod*)malloc(sizeof(Nod));   //creez un nod nou
        nod_nou->x = c;  //pun in acesta caracterul extras 
        nod_nou->next = banda->finger->next;  //adaug nodul in dreapta nodului curent
        banda->finger->next = nod_nou;
        nod_nou->prev = banda->finger;
        if (nod_nou->next != NULL) {
           nod_nou->next->prev = nod_nou;
        }
        banda->finger = banda->finger->next; 
    }
    if (strncmp(temp->operatie, "INSERT_LEFT ", 12) == 0) {
        char c;
        int len = strlen(temp->operatie);
        c = temp->operatie[len - 1];
        if (banda->finger->prev != sentinel) {  //daca nu suntem la capatul listei
            Nod* nod_nou = (Nod*)malloc(sizeof(Nod)); //creez nod nou
            nod_nou->x = c;  //adaug caracterul
            nod_nou->prev = banda->finger->prev;  //pozitionez nodul in stanga nodului curent
            banda->finger->prev->next = nod_nou;
            banda->finger->prev = nod_nou;
            nod_nou->next = banda->finger;

            banda->finger = banda->finger->prev;
        }
        else { //daca suntem la capatul listei printam eroare
            fprintf(f, "ERROR\n");
        }
    }
    free(temp);  //eliberam memoria alocata
}

int main () {
    int n;

    Nod *sentinel = (Nod*)malloc(sizeof(Nod));  //creez santinela pentru lista
    sentinel->x = '0';
    sentinel->prev = sentinel;
    sentinel->next = (Nod*)malloc(sizeof(Nod));
    sentinel->next->x = '#';
    sentinel->next->prev = sentinel;
    sentinel->next->next = NULL;

    Banda *banda;   //creez banda pentru lista si deget
    banda = (Banda*)malloc(sizeof(Banda));
    banda->banda = sentinel;
    banda->finger = sentinel->next;
    
    Coada *q = (Coada*)malloc(sizeof(Coada));  //creez o coada 
    q->rear = NULL;
    q->front = q->rear;

    Stiva_undo *s1 = (Stiva_undo*)malloc(sizeof(Stiva_undo));  //creez stiva pentru undo
    s1->top = NULL;

    Stiva_redo *s2 = (Stiva_redo*)malloc(sizeof(Stiva_redo));  //creez stiva pentru redo
    s2->top = NULL;

    char ope[20];
    char* word;
    char delimitator[] = "\n";

    FILE *file;  
    file = fopen("tema1.in", "r");  //deschid fisierul pentru citit

    FILE *f;
    f = fopen("tema1.out", "w");  //deschid fisierul pentru scris 
 
    fscanf(file, "%d", &n);  //citesc primul rand
    while (fgets(ope, sizeof(ope), file) != NULL) {  //citesc celelalte randuri
        word = strtok(ope, delimitator);  //cuvant pana cand intalneste new line
        while (word != NULL) {
        if (strcmp(word, "MOVE_RIGHT") == 0) {
            enqueue(q, ope); //adaug in coada 
        }
        if (strcmp(word, "MOVE_LEFT") == 0) {
            enqueue(q, ope); //adaug in coada 
        }
        if (strcmp(word, "EXECUTE") == 0) {
            int a = 0;
            char op[15];
            dequeue(q, banda, sentinel, f, &a, op);
            if (a == 1) {
                if (strcmp(op, "MOVE_RIGHT") == 0) {
                    pushUndo(s1, banda->finger->prev); //adaug in stiva pentru undo
                }
                if (strcmp(op, "MOVE_LEFT") == 0) {
                    pushUndo(s1, banda->finger->next); //adaug in stiva pentru undo
                }
                if (strncmp(op, "WRITE", 5) == 0) {
                    while (s1->top != NULL) 
                        popUndo(s1); //golesc stiva 
                    while (s2->top != NULL)
                        popRedo(s2); //golesc stiva
                }
            }
        } 
        if (strcmp(word, "UNDO") == 0) {
            undo(s1, banda, s2); //apelez functia undo
        }
        if (strcmp(word, "REDO") == 0) {
            redo(s2, banda, s1); //apelez functia redo
        }
        if (strncmp(word, "MOVE_RIGHT_CHAR ", 16) == 0) {
            enqueue(q, ope); //adaug in coada 
        } 
        if (strncmp(word, "MOVE_LEFT_CHAR ", 15) == 0) {
            enqueue(q, ope); //adaug in coada 
        }
        if (strncmp(word, "WRITE ", 6) == 0) {
            enqueue(q, ope); //adaug in coada 
        }
        if (strncmp(word, "INSERT_RIGHT ", 13) == 0) {
            enqueue(q, ope); //adaug in coada 
        }
        if (strncmp(word, "INSERT_LEFT ", 12) == 0) {
            enqueue(q, ope); //adaug in coada 
        }
        if (strcmp(word, "SHOW") == 0) {
            Nod* p;
            for (p=sentinel->next; p!= NULL; p = p->next) {
                if (p->x == banda->finger->x && p->next == banda->finger->next && p->prev == banda->finger->prev) {
                    fprintf(f, "|%c|", p->x); //afisez degetul
                }
                else {
                    fprintf(f, "%c", p->x); //afisez celelalte caractere 
                }
            }
            fprintf(f, "\n");
        }
        if (strncmp(word, "SHOW_CURRENT", 12) == 0) {
            fprintf(f, "%c\n", banda->finger->x);  //afisez degetul
        }
        word = strtok(NULL, delimitator);
        }
    } 

    Nod* temp;
    while (sentinel != NULL) {  //eliberez memoria pentru lista
        temp = sentinel;
        sentinel = sentinel->next;
        free(temp);
    }
    free(sentinel);

    Stivanod *temps;  //eliberez memoria pentru stiva undo
    while (s1->top != NULL) {
        temps = s1->top;
        s1->top = s1->top->next;
        free(temps);
    }
    free(s1);

    while (s2->top != NULL)  {  //eliberez memoria pentru stiva redo
        temps = s2->top;
        s2->top = s2->top->next;
        free(temps);
    }
    free(s2);

    Coadanod* currnod; //eliberez memoria pentru coada
    currnod = q->front;
    while (currnod != NULL) {
        Coadanod* nextnod = currnod->next;
        free(currnod);
        currnod = nextnod;
    } 
    free(q);
    
    free(banda); //eliberez banda 

    fclose(f); //inchid fisierele
    fclose(file);
    
    return 0;
}