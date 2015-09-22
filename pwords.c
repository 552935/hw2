#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXWORD 1024
pthread_t thread[4];
pthread_mutex_t lock;

typedef struct dict {
  char *word;
  int count;
  struct dict *next;
} dict_t;

typedef struct wrd
{
	dict_t *w;
    char wrd_buffer[MAXWORD];
	
}wrd_t;


char *make_word( char *word ) {
  return strcpy( malloc( strlen( word )+1 ), word );
}

void *make_dict(void*word) {
  

   pthread_mutex_lock (&lock);
  dict_t *nd = (dict_t *) malloc( sizeof(dict_t) );
  nd->word = make_word( word );
  nd->count = 1;
  nd->next = NULL;
  pthread_mutex_unlock (&lock);
  return nd;
}

void *insert_word( void * m){

wrd_t *p = malloc(sizeof (*p));
p= (wrd_t *) m;
dict_t *d = p->w;
char * word = p->wrd_buffer;

  dict_t *nd;
  dict_t *pd = NULL;	
  dict_t *di = d;	

  while(di && ( strcmp(word, di->word ) >= 0) ) { 
    if( strcmp( word, di->word ) == 0 ) { 
      di->count++;		
      return d;			
    }
    pd = di;			
    di = di->next;
  }

  void * something ;
  pthread_create(&thread[3], NULL, make_dict, word);
  pthread_join(thread[3], &something);

  dict_t *element = malloc(sizeof(*element));
  element = (dict_t *) something;
  
  element->next = di;		
  if (pd) {
    pd->next = element;
    return d;			
  }
 
  return element;
}


void * print_dict( dict_t * d) {

    
	
  while (d) {
    printf("[%d] %s\n", d->count, d->word);
    d = d->next;
  }
}

int get_word( char *buf, int n, FILE *infile) {
  int inword = 0;
  int c;  
  while( (c = fgetc(infile)) != EOF ) {
    if (inword && !isalpha(c)) {
      buf[inword] = '\0';	
      return 1;
    } 
    if (isalpha(c)) {
      buf[inword++] = c;
    }
  }
  return 0;			
}





void *words( void * input ) {

  FILE *infile = (FILE * ) input;
  
  dict_t *wd = NULL;
  char wordbuf[MAXWORD] ;

  void * i;

  wrd_t *r = malloc(sizeof (*r));
  


  
  while( get_word( wordbuf, MAXWORD, infile ) ) {

  	r->w = wd;

 	strcpy(r->wrd_buffer, wordbuf); 
 	pthread_create(&thread[2], NULL, insert_word, r);
  	pthread_join(thread[2], &i);
  	wd = i;

  }

 
  free (r);
  return  wd;
}

int
main( int argc, char *argv[] ) {
  dict_t *d = NULL;
  FILE *infile = stdin;
  if (argc >= 2) {
    infile = fopen (argv[1],"r");
  }
  if( !infile ) {
    printf("Unable to open %s\n",argv[1]);
    exit( EXIT_FAILURE );
  }

 void * s;

pthread_create(&thread[0], NULL, words,  infile);
 pthread_join(thread[0], &s);

 pthread_create(&thread[1], NULL, print_dict,  s);

 
 pthread_join(thread[1], NULL);

  fclose( infile );
}
