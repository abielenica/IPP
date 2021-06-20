/** @file
  Bezpieczna alokacja pamięci

  @author Agata Bielenica <ab417724@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef __POLYNOMIALS_MEMORY_H__
#define __POLYNOMIALS_MEMORY_H__

#include <stdlib.h>
/**
 * Próbuje zaalokować daną ilość pamięci za pomocą funkcji malloc.
 * Jeśli malloc przyjmie wartość NULL, przerywa działanie programu.
 * W przeciwnym przypadku przekazuje wskaźnik na zaalokowaną pamięć.
 * @param[in] size : liczba bajtów do zaalokowania
 * @return wskaźnik na zaalokowaną pamięć
 */
static inline void *NullProofMalloc(size_t size) {
  void *pointer = malloc(size);
  if (pointer == NULL)
    exit(EXIT_FAILURE);
  return pointer;
}

/**
 * Próbuje zrealokować daną pamięć za pomocą funkcji realloc.
 * Jeśli realloc przyjmie wartość NULL, przerywa działanie programu.
 * W przeciwnym przypadku przekazuje wskaźnik na zaalokowaną pamięć.
 * @param[in] size : liczba bajtów do zaalokowania
 * @param[in] pointer : wskaźnik na pamięć do zrealokowania
 * @return wskaźnik na zaalokowaną pamięć
 */
static inline void *NullProofRealloc(void* pointer, size_t size) {
  void *result = realloc(pointer, size);
  if (result == NULL)
    exit(EXIT_FAILURE);
  return result;
}

#endif /* __POLYNOMIALS_MEMORY_H__ */
