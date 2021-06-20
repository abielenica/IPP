/** @file
  Implementacja klasy wielomianów rzadkich wielu zmiennych

  @author Agata Bielenica <ab417724@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "poly.h"
#include "memory.h"

void PolyDestroy(Poly *p) {
    if (PolyIsCoeff(p))
        return;
    for (size_t i = 0; i < p->size; i++) {
        MonoDestroy(&p->arr[i]);
    }
    free(p->arr);
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) != PolyIsCoeff(q))
        return false;
    if (PolyIsCoeff(p))
        return p->coeff == q->coeff;
    if (p->size != q->size)
        return false;
    for (size_t i = 0; i < p->size; i++) {
        if (p->arr[i].exp != q->arr[i].exp || !(PolyIsEq(&p->arr[i].p, &q->arr[i].p)))
            return false;
    }
    return true;
}

/**
 * Dodaje wielomian stały do wielomianu o dodatnim stopniu.
 * @param[in] coeff_p : wielomian stały @f$p@f$
 * @param[in] poly_q : wielomian dodatniego stopnia @f$q@f$
 * @return @f$p + q@f$
 */
static Poly AddCoeffAndNonCoeff(const Poly *coeff_p, const Poly *poly_q) {
    assert(PolyIsCoeff(coeff_p) && !PolyIsCoeff(poly_q));
    size_t count = poly_q->size + 1;
    Mono *monos_array = NullProofMalloc(count * sizeof(Mono));
    for (size_t i = 0; i < poly_q->size; i++) {
        monos_array[i] = MonoClone(&poly_q->arr[i]);
    }
    monos_array[count - 1] = (Mono){.exp = 0, .p = PolyFromCoeff(coeff_p->coeff)};
    Poly result = PolyAddMonos(count, monos_array);
    free(monos_array);
    return result;
}

/**
 * Dodaje dwa wielomiany o dodatnich stopniach.
 * @param[in] poly_p : wielomian stały @f$p@f$
 * @param[in] poly_q : wielomian dodatniego stopnia @f$q@f$
 * @return @f$p + q@f$
 */
static Poly AddTwoNonCoeffs(const Poly *poly_p, const Poly *poly_q) {
    assert(!PolyIsCoeff(poly_p) && !PolyIsCoeff(poly_q));
    size_t count = poly_p->size + poly_q->size;
    Mono *monos_array = NullProofMalloc(count * sizeof(Mono));
    for (size_t i = 0; i < poly_p->size; i++) {
        monos_array[i] = MonoClone(&poly_p->arr[i]);
    }
    for (size_t i = 0; i < poly_q->size; i++) {
        monos_array[i + poly_p->size] = MonoClone(&poly_q->arr[i]);
    }
    Poly result = PolyAddMonos(count, monos_array);
    free(monos_array);
    return result;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return PolyFromCoeff(p->coeff + q->coeff);
    if (!PolyIsCoeff(p) && !PolyIsCoeff(q))
        return AddTwoNonCoeffs(p, q);
    if (PolyIsCoeff(p))
        return AddCoeffAndNonCoeff(p, q);
    else
        return AddCoeffAndNonCoeff(q, p);
}

Poly PolyClone(const Poly *p) {
    if (PolyIsCoeff(p))
        return (Poly){.arr = NULL, .coeff = p->coeff};
    Poly poly_copy = {.size = p->size, .arr = NullProofMalloc(p->size * sizeof(Mono))};
    for (size_t i = 0; i < p->size; i++) {
        poly_copy.arr[i] = MonoClone(&(p->arr[i]));
    }
    return poly_copy;
}

/**
 * Porównuje ze sobą wykładniki dwóch jednomianów
 * w sposób, jakiego wymaga funkcja qsort.
 * @param[in] mono1 : jednomian o wykładniku @f$e_1@f$
 * @param[in] mono2 : jednomian o wykładniku @f$e_2@f$
 * @return @f$e_1 - e_2@f$
 */
static int CompareMonosByExp(const void *mono1, const void *mono2) {
    Mono m1 = *((Mono *)mono1);
    Mono m2 = *((Mono *)mono2);
    return m1.exp - m2.exp;
}

/**
 * Rekurencyjnie sprawdza, czy jednomian jest tożsamościowo równy zeru.
 * @param[in] mono : jednomian
 * @return Czy jednomian jest równy zeru?
 */
bool DeepMonoIsZero(const Mono *mono) {
    if (PolyIsCoeff(&mono->p))
        return PolyIsZero(&mono->p);
    for (size_t i = 0; i < mono->p.size; i++) {
        if (!DeepMonoIsZero(&mono->p.arr[i]))
            return false;
    }
    return true;
}

/**
 * Dodaje dwa jednomiany o równych wykładnikach.
 * @param[in] m1 : jednomian @f$m_1@f$
 * @param[in] m2 : jednomian @f$m_2@f$
 * @return @f$m_1 + m_2@f$
 */
static Mono AddSameExpMonos(const Mono *m1, const Mono *m2) {
    assert(m1->exp == m2->exp);
    return (Mono){.exp = m1->exp, .p = PolyAdd(&m1->p, &m2->p)};
}

/**
 * Rekurencyjnie sprawdza, czy dany wielomian jest wielomianem stałym.
 * Jeśli tak jest, dodatkowo zapisuje współczynnik wielomianu stałego na danej zmiennej.
 * W przeciwnym przypadku zapisuje na niej wartość zero.
 * @param[in] poly : wielomian
 * @param[out] result_coeff : zmienna do zapisania współczynnika
 * @return Czy wielomian jest wielomianem stałym?
 */
static bool DeepPolyIsCoeff(const Poly *poly, poly_coeff_t *result_coeff) {
    *result_coeff = 0;
    if (PolyIsCoeff(poly)) {
        *result_coeff = poly->coeff;
        return true;
    }
    if (poly->size == 0) {
        return true;
    }
    for (size_t i = 0; i < poly->size; i++) {
        assert(i == 0 || poly->arr[i].exp > poly->arr[i - 1].exp);
    }
    if (poly->arr[0].exp != 0) {
        return false;
    }
    for (size_t i = 1; i < poly->size; i++) {
        if (!DeepMonoIsZero(&poly->arr[i]))
            return false;
    }
    return DeepPolyIsCoeff(&poly->arr[0].p, result_coeff);
}

/**
 * Sumuje tablicę jednomianów i zapisuje wynik
 * w tablicy danego wielomianu wynikowego.
 * @param[in] count : rozmiar tablicy jednomianów
 * @param[in] monos_arr : tablica jednomianów
 * @param[out] result : wielomian wynikowy
 */
static void FillInPolyAddMonosResult(size_t count, Mono *monos_arr, Poly *result) {
    size_t monos_i = 0;
    size_t result_i = 0;
    while (monos_i < count) {
        result->arr[result_i] = MonoClone(&monos_arr[monos_i]);
        monos_i++;
        while (monos_i < count &&
               monos_arr[monos_i].exp == monos_arr[monos_i - 1].exp) {
            Mono prev_res = result->arr[result_i];
            result->arr[result_i] = AddSameExpMonos(&result->arr[result_i], &monos_arr[monos_i]);
            MonoDestroy(&prev_res);
            if (DeepMonoIsZero(&result->arr[result_i])) {
                Mono prev_result = result->arr[result_i];
                result->arr[result_i] = (Mono){.exp = prev_result.exp, .p = PolyZero()};
                MonoDestroy(&prev_result);
            }
            monos_i++;
        }
        if (!DeepMonoIsZero(&result->arr[result_i]))
            result_i++;
    }
    result->size = result_i;
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if (count == 0)
        return PolyZero();
    Mono *monos_copy =
        memcpy(NullProofMalloc(count * sizeof(Mono)), monos, count * sizeof(Mono));
    qsort(monos_copy, count, sizeof(Mono), CompareMonosByExp);
    Poly result = {.size = count, .arr = NullProofMalloc(count * sizeof(Mono))};
    FillInPolyAddMonosResult(count, monos_copy, &result);
    poly_coeff_t new_coeff;
    if (DeepPolyIsCoeff(&result, &new_coeff)) {
        PolyDestroy(&result);
        result.arr = NULL;
        result.coeff = new_coeff;
    }
    for (size_t i = 0; i < count; i++) {
        MonoDestroy(&monos_copy[i]);
    }
    free(monos_copy);
    return result;
}

/**
 * Mnoży dwa jednomiany.
 * @param[in] m1 : jednomian @f$m_1@f$
 * @param[in] m2 : jednomian @f$m_2@f$
 * @return @f$m_1 * m_2@f$
 */
static Mono MonoMul(const Mono *m1, const Mono *m2) {
    return (Mono){.exp = m1->exp + m2->exp, .p = PolyMul(&m1->p, &m2->p)};
}

/**
 * Mnoży dwa wielomiany dodatnich stopni.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p * q@f$
 */
static Poly MultiplyTwoNonCoeffs(const Poly *p, const Poly *q) {
    assert(!PolyIsCoeff(p) && !PolyIsCoeff(q));
    size_t count = 0;
    Mono *monos = NullProofMalloc((p->size * q->size) * sizeof(Mono));
    for (size_t i = 0; i < p->size; i++) {
        for (size_t j = 0; j < q->size; j++) {
            Mono mul_res = MonoMul(&p->arr[i], &q->arr[j]);
            if (!PolyIsZero(&mul_res.p)) {
                monos[count] = MonoClone(&mul_res);
                count++;
            }
            MonoDestroy(&mul_res);
        }
    }
    Poly result = PolyAddMonos(count, monos);
    free(monos);
    return result;
}

/**
 * Mnoży wielomian dodatniego stopnia przez wielomian stały.
 * @param[in] coeff : wielomian stały @f$c@f$
 * @param[in] poly : wielomian dodatniego stopnia @f$p@f$
 * @return @f$c * p@f$
 */
static Poly MultiplyCoeffAndNonCoeff(const Poly *coeff, const Poly *poly) {
    assert(PolyIsCoeff(coeff) && !PolyIsCoeff(poly));
    if (PolyIsZero(coeff))
        return PolyZero();
    Poly result = {.size = poly->size, .arr = NullProofMalloc(poly->size * sizeof(Mono))};
    Mono mono_from_coeff = MonoFromPoly(coeff, 0);
    size_t result_i = 0;
    for (size_t i = 0; i < poly->size; i++) {
        result.arr[result_i] = MonoMul(&mono_from_coeff, &poly->arr[i]);
        if (!PolyIsZero(&result.arr[result_i].p))
            result_i++;
    }
    result.size = result_i;
    if (result_i == 0) {
        free(result.arr);
        return PolyZero();
    }
    return result;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    if (PolyIsZero(p) || PolyIsZero(q))
        return PolyZero();
    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        Poly result = PolyFromCoeff(p->coeff * q->coeff);
        return result;
    }
    if (!PolyIsCoeff(p) && !PolyIsCoeff(q)) {
        return MultiplyTwoNonCoeffs(p, q);
    }
    if (PolyIsCoeff(p)) {
        return MultiplyCoeffAndNonCoeff(p, q);
    } else {
        return MultiplyCoeffAndNonCoeff(q, p);
    }
}

Poly PolyNeg(const Poly *p) {
    Poly neg_poly = PolyFromCoeff(-1);
    return PolyMul(p, &neg_poly);
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly negative_q = PolyNeg(q);
    Poly result = PolyAdd(p, &negative_q);
    PolyDestroy(&negative_q);
    return result;
}

/**
 * Podnosi daną liczbę do określonej potęgi.
 * @param[in] base : liczba potęgowana @f$x@f$
 * @param[in] exp : wykładnik @f$n@f$
 * @return @f$x^n@f$
 */
static Poly Exponentiate(poly_coeff_t base, poly_exp_t exp) {
    poly_coeff_t result = 1;
    while (exp > 0) {
        if (exp % 2 == 1)
            result = result * base;
        exp /= 2;
        base = base * base;
    }
    return PolyFromCoeff(result);
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p))
        return (Poly){.arr = NULL, .coeff = p->coeff};
    Poly result = PolyZero();
    for (size_t i = 0; i < p->size; i++) {
        Poly multiplier = Exponentiate(x, p->arr[i].exp);
        Poly poly_mul_res = PolyMul(&multiplier, &p->arr[i].p);
        Poly prev_result = result;
        result = PolyAdd(&result, &poly_mul_res);
        PolyDestroy(&poly_mul_res);
        PolyDestroy(&prev_result);
    }
    return result;
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsZero(p))
        return -1;
    if (PolyIsCoeff(p))
        return 0;
    poly_exp_t max = 0;
    for (size_t i = 0; i < p->size; i++) {
        poly_exp_t temp = PolyDeg(&p->arr[i].p) + p->arr[i].exp;
        if (temp > max)
            max = temp;
    }
    return max;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    if (PolyIsZero(p))
        return -1;
    if (PolyIsCoeff(p))
        return 0;
    if (var_idx == 0)
        return p->arr[p->size - 1].exp;
    poly_exp_t max = 0;
    for (size_t i = 0; i < p->size; i++) {
        poly_exp_t temp = PolyDegBy(&p->arr[i].p, var_idx - 1);
        if (temp > max)
            max = temp;
    }
    return max;
}