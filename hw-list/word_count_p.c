/*
 * Implementation of the word_count interface using Pintos lists and pthreads.
 *
 * You may modify this file, and are expected to modify it.
 */

/*
 * Copyright © 2021 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PINTOS_LIST
#error "PINTOS_LIST must be #define'd when compiling word_count_lp.c"
#endif

#ifndef PTHREADS
#error "PTHREADS must be #define'd when compiling word_count_lp.c"
#endif

#include "word_count.h"

void init_words(word_count_list_t* wclist) { /* TODO */
  list_init(&wclist->lst);
  //wclist->lock = PTHREAD_MUTEX_INITIALIZER;//only works for global/static mutexes for direct struct members
  pthread_mutex_init(&wclist->lock, NULL);//works for heap-allocated mutexes
}

size_t len_words(word_count_list_t* wclist) {
  /* TODO */
  pthread_mutex_lock(&wclist->lock);
  size_t length_wclist = list_size(&wclist->lst);
  pthread_mutex_unlock(&wclist->lock);
  return length_wclist;
}

word_count_t* find_word(word_count_list_t* wclist, char* word) {
  /* TODO */
  struct list_elem *e;
  for(e = list_begin(&wclist->lst); e != list_end(&wclist->lst); e = list_next(e)){
    word_count_t *wc = list_entry(e, word_count_t, elem);
    if(strcmp(wc->word,word) == 0){//if find the word, ite will jump to the next line 
      return wc;
    }
  }
  //if jump to here, means can't find the word, the word is fresh
  return NULL;
}

word_count_t* add_word(word_count_list_t* wclist, char* word) {
  /* TODO */
  if(!word || strlen(word)==0){
    return NULL;
  }
  pthread_mutex_lock(&wclist->lock);
  word_count_t *wc = find_word(wclist, word);
  if(wc == NULL){//the word hasn't exist, so it really need to add to the list
    wc = (word_count_t*)malloc(sizeof(word_count_t));//first we need to allocate the heap memory to the word(and the struct)
    if(wc == NULL){//in case the memory allocation fail
      pthread_mutex_lock(&wclist->lock);
      fprintf(stderr, "Memory allocation failed!!!");
      exit(1);
    }else{
      wc->word = word;
      wc->count = 1;
      list_push_front(&wclist->lst,&wc->elem);
    }
    pthread_mutex_unlock(&wclist->lock);
    return wc;
  }
  //if found the word has already exist, it would jump to here
  wc->count++;
  pthread_mutex_unlock(&wclist->lock);
  return wc;
}

void fprint_words(word_count_list_t* wclist, FILE* outfile) {
  /* TODO */
  /* Please follow this format: fprintf(<file>, "%i\t%s\n", <count>, <word>); */
  pthread_mutex_lock(&wclist->lock);
  struct list_elem *e;
  for(e = list_begin(&wclist->lst);e != list_end(&wclist->lst); e = list_next(e)){
    word_count_t *wc = list_entry(e, word_count_t, elem);
    fprintf(outfile, "%i\t%s\n",wc->count,wc->word);
  }
  pthread_mutex_unlock(&wclist->lock);
}

static bool list_less(const struct list_elem *a, const struct list_elem *b, void* aux){
  word_count_t *wc1 = list_entry(a, word_count_t, elem);
  word_count_t *wc2 = list_entry(b, word_count_t, elem);
  bool (*less)(const word_count_t*, const word_count_t*) = aux;
  return less(wc1, wc2);
}

void wordcount_sort(word_count_list_t* wclist,
                    bool less(const word_count_t*, const word_count_t*)) {
  /* TODO */
  pthread_mutex_lock(&wclist->lock);
  list_sort(&wclist->lst,list_less,less);
  pthread_mutex_unlock(&wclist->lock);
}
