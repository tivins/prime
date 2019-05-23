/*
    libln

    This library is about large numbers.
    This numbers are larges integers, greater than `INT_MAX`.

    There is no dependency equired to build it, except a
    compatible C compiler.

    The digits are stored as integers. The convertion to
    their ASCII form is only performed to display
    them (e.g.: `digit + '0'`).

*/

#include "ln.h"
#include <stdio.h>
#include <string.h>

void ln_init(ln_t * _n) {
    _n->int_cap = 0;
    _n->integer = NULL;
    _n->int_sz = 0;
    _n->negative = 0;
}

/* clear the data of number but keep the memory allocated. */
void ln_clear(ln_t * _n) {
    _n->int_sz = 0;
    _n->negative = 0;
}

/* clear all data, included the allocation of memory. */
void ln_free(ln_t * _n) {
    if (_n->integer) free(_n->integer);
    ln_init(_n);
}

void ln_negate_copy(ln_t * _src, ln_t * _dst) {
    ln_copy(_src, _dst);
    ln_negate(_dst);
}

void ln_copy(ln_t * _src, ln_t * _dst) {
    _dst->int_cap = _src->int_cap;
    _dst->integer = malloc(sizeof(char) * _src->int_cap);
    memcpy(_dst->integer, _src->integer, _src->int_cap);
    _dst->int_sz = _src->int_sz;
    _dst->negative = _src->negative;
}

void ln_negate(ln_t * _n) {
    _n->negative = !_n->negative;
}

size_t ln_max_sz(ln_t * _a, ln_t * _b) {
    return _a->int_sz > _b->int_sz ? _a->int_sz : _b->int_sz;
}

char ln_at(ln_t * _n, size_t _i) {
    if (_i >= _n->int_sz) return -1;
    return _n->integer[_i];
}

char ln_last_at(ln_t * _n, size_t _i) {
    if (_i >= _n->int_sz) return -1;
    return _n->integer[_n->int_sz - _i - 1];
}

void ln_set(ln_t * _n, size_t _i, int _digit) {
    /* if (_i >= _n->int_sz) return; */
    _n->integer[_i] = _digit;
}

void ln_resize(ln_t * _n, size_t _cap) {
    ln_reserve(_n, _cap);
    while (_n->int_sz < _cap) {
        _n->integer[_n->int_sz] = -1;
        _n->int_sz++;
    }
}

void ln_reserve(ln_t * _n, size_t _cap) {
    if (_n->int_cap >= _cap) return;
    if (_n->int_sz == 0) {
        _n->int_cap = _cap;
        _n->integer = malloc(sizeof(char) * _n->int_cap);
    }
    _n->int_cap *= 2;
    _n->integer = realloc(_n->integer, sizeof(char) * _n->int_cap);
}

void ln_append(ln_t * _n, int _digit) {
    ln_reserve(_n, _n->int_sz + 1);
    _n->integer[_n->int_sz] = _digit;
    _n->int_sz++;
}

void ln_prepend(ln_t * _n, int _digit) {
    ln_reserve(_n, _n->int_sz + 1);
    memmove(_n->integer + 1, _n->integer, _n->int_sz);
    _n->integer[0] = _digit;
    _n->int_sz++;
}

void ln_append_str(ln_t * _n, const char * _str, size_t _l) {
    const char * ptr = _str;
    char neg = *ptr == '-';
    if (neg) ptr++;
    _l = _l ? (_l - neg) : strlen(_str);
    ln_reserve(_n, _n->int_sz + _l);
    while (ptr < _str + _l) {
        ln_append(_n, *ptr - '0');
        ptr++;
    }
}

void ln_append_int(ln_t * _n, int _val) {
    static char intg[33];
    if (_val < 0) { _val = -_val; _n->negative = 1; }
    sprintf(intg, "%d", _val);
    ln_append_str(_n, intg, 0);
}

void ln_trim(ln_t * _n) {
    size_t it, num_z = 0;
    for (it = 0; it < _n->int_sz; it++) {
        if (_n->integer[it] != 0) break;
        num_z++;
    }
    if (!num_z) return;
    memmove(_n->integer, _n->integer + num_z, _n->int_sz - num_z);
    _n->int_sz -= num_z;
}

int ln_is_zero(ln_t * _n) {
    return ((_n->int_sz == 0) || (_n->int_sz == 1 && _n->integer[0] == 0));
}


/* ln_Lesser (a < b), ln_Equal (a = b), ln_Greater (a > b) */
int ln_cmp(ln_t * _a, ln_t * _b) {
    int va, vb; /* numeric values for each char */
    size_t it = 0;

    if (_a->negative && !_b->negative) return ln_Lesser;
    if (!_a->negative && _b->negative) return ln_Greater;
    if (_a->int_sz > _b->int_sz) return ln_Greater;
    if (_a->int_sz < _b->int_sz) return ln_Lesser;
    /* At this point, _a and _b have the same size and same sign */

    while (1) {
        /* get numeric values for chars. */
        va = ln_at(_a, it);
        vb = ln_at(_b, it);
        if (va < 0 && vb < 0) break;
        /* This comparison works because both have same size. */
        if (va < vb) return ln_Lesser;
        if (va > vb) return ln_Greater;
        it++;
    }
    return ln_Equal;
}

void ln_dump(ln_t * _n)
{
    size_t n;
    printf("ln_dump:\n");
    for (n=0;n<_n->int_sz;n++) {
        printf(" %04d ", _n->integer[n]);
    }
    printf("\n");
}

char * ln_c_str(ln_t * _n) {
    size_t it;
    char * out = malloc(sizeof(char) * (_n->negative + _n->int_sz + 1));
    if (_n->negative) out[0] = '-';
    for (it = 0; it < _n->int_sz; ++it) {
        out[it + _n->negative] = _n->integer[it] + '0';
    }
    out[it + _n->negative] = '\0';
    return out;
}

void ln_show(ln_t * _n, const char * _sfx) {
    char * ns1 = ln_c_str(_n);
    printf("%s", ns1);
    if (_sfx) printf("%s", _sfx);
    free(ns1);
}

void ln_inc(ln_t * _n) {
    ln_t _1, cpy;

    ln_init(&_1);
    ln_append(&_1, 1);

    ln_copy(_n, &cpy);
    ln_clear(_n);
    ln_add(_n, &cpy, &_1);

    ln_free(&cpy);

    ln_free(&_1);
}

void ln_dec(ln_t * _n) {
    ln_t _1, cpy;
    ln_copy(_n, &cpy);
    ln_init(&_1);
    ln_append(&_1, 1);
    ln_clear(_n);
    ln_sub(_n, &cpy, &_1);
    ln_free(&_1);
    ln_free(&cpy);
}

void ln_add(ln_t * _out, ln_t * _a, ln_t * _b) {
    int va, vb, vt; /* numeric values for each char */
    int base, carry;
    size_t it;
    ln_t cpy;
    if (_a->negative && !_b->negative) {
        ln_negate_copy(_a, &cpy);
        ln_sub(_out, &cpy, _b);
        ln_negate(_out);
        ln_free(&cpy);
        return;
    }
    if (!_a->negative && _b->negative) {
        ln_negate_copy(_b, &cpy);
        ln_sub(_out, _a, &cpy);
        ln_free(&cpy);
        return;
    }
    it = 0;
    base = 10;
    carry = 0;
    ln_reserve(_out, ln_max_sz(_a, _b));
    while (1) {
        /* get numeric values for chars. */
        va = ln_last_at(_a, it);
        vb = ln_last_at(_b, it);
        /* check if we reached the end of strings. */
        if (va < 0 && vb < 0) {
            /* To append the carry if it remains. */
            /* [todo] Check if it works in all cases. */
            if (carry) ln_prepend(_out, carry);
            break;
        }
        /* If one of the value are negative, set to zero. One of them have not
           chars any more. */
        if (va < 0) va = 0;
        if (vb < 0) vb = 0;
        /* The operation for the char. */
        vt = va + vb + carry;
        /* Reset carry because it was used in the operation. */
        carry = 0;
        /* Keep remainder of value. */
        while (vt >= base) {
            carry++;
            vt -= base;
        }
        ln_prepend(_out, vt);
        ++it;
    }
    if (_a->negative && _b->negative) ln_negate(_out);
}

void ln_sub_int(ln_t * _out, ln_t * _a, int _b) {
    ln_t nint;
    ln_init(&nint);
    ln_init(_out);
    ln_append_int(&nint, _b);
    ln_sub(_out, _a, &nint);
    ln_free(&nint);
}

void ln_sub(ln_t * _out, ln_t * _a, ln_t * _b) {
    /* LNLOG(); ln_show(_a, " - "); ln_show(_b, "\n"); */

    int va, vb; /* numeric values for each char */
    int base, carry;
    size_t it;
    ln_t cpy;

    /* a is negative , b is positive : 3 - -2 = 3 + 2 = 5 => a + -b */
    if (!_a->negative && _b->negative) {
        ln_negate_copy(_b, &cpy);
        ln_add(_out, _a, &cpy);
        ln_free(&cpy);
        return;
    }
    /* a is lesser than b : 3 - 6 = -(6 - 3) = -3 => -(b - a) */
    if (ln_cmp(_a, _b) == ln_Lesser) {
        ln_sub(_out, _b, _a);
        ln_negate(_out);
        return;
    }

    /* now, we compute and don't take care about number's signs. */

    it = 0;
    base = 10;
    carry = 0;
    while (1) {
        /* get numeric values for chars. */
        va = ln_last_at(_a, it);
        vb = ln_last_at(_b, it);
        if (va < 0 && vb < 0) {
            /* [todo] Check is carry remains...? */
            break;
        }
        /* If one of the value are negative, set to zero. One of them have not
           chars any more. */
        if (va < 0) va = 0;
        if (vb < 0) vb = 0;

        vb += carry;
        carry = 0;

        while (va < vb) { va += base; carry++; }
        ln_prepend(_out, va - vb);
        it++;
    }
    ln_trim(_out);

    /* a and b are negative : -3 - -2 = -(3 - 2) = -1 = -(abs(a) - abs(b)) */
    if (_a->negative && _b->negative) ln_negate(_out);
}

void ln_mul_int(ln_t * _out, ln_t * _a, int _b) {
    ln_t nint;
    ln_init(&nint);
    ln_init(_out);
    ln_append_int(&nint, _b);
    ln_mul(_out, _a, &nint);
    ln_free(&nint);
}

void ln_mul_str(ln_t * _out, ln_t * _a, const char * _str, size_t _len) {
    ln_t nint;
    ln_init(&nint);
    ln_append_str(&nint, _str, _len);
    ln_mul(_out, _a, &nint);
    ln_free(&nint);
}

void ln_mul(ln_t * _out, ln_t * _a, ln_t * _b) {

    int carry, va, vb, num_cur, num_new, newval;
    int b_i, a_i, base = 10;
    size_t alen = _a->int_sz;
    size_t blen = _b->int_sz;
    int debug = 0;

    /* first quick checks... */
    if (ln_is_zero(_a) || ln_is_zero(_b)) {
        /* hum... clear ? _out.append(0) ? both ? do nothing ? */
        return;
    }

    ln_resize(_out, alen + blen - 1);

    for (b_i = blen-1; b_i >= 0; b_i--) { /* for all digits in b */

        carry = 0; /* reset the carry */
        vb = ln_at(_b, b_i);
        if (debug) printf("_b[%d]=%d\n", b_i, vb);

        for (a_i = alen-1; a_i >= 0; a_i--) { /* for all digits in a */

            /* get numeric values for chars. */
            va = ln_at(_a, a_i);
            if (debug) printf(" > _a[%d]=%d", a_i, va);

            /* If one of the value are negative, set to zero. One of them have not
               chars any more. */
            if (va < 0) va = 0;
            if (vb < 0) vb = 0;

            /* Récupère la valeur numérique pour l'index : */
            num_cur = ln_at(_out, a_i + b_i);
            if (num_cur < 0) num_cur = 0;

            if (debug) printf(" > _out[%d]=%d", a_i + b_i, num_cur);


            /* Notre valeur à ajouter est la multiplication des 2 + la retenue : */
            num_new = carry + va * vb;
            if (debug) printf(" > num_new = %d", num_new);

            newval = num_cur + num_new;
            if (debug) printf(" > newval = %d", newval);

            carry = newval / base;
            if (debug) printf(" > carry = %d", carry);

            ln_set(_out, a_i + b_i, newval - carry * base);
            if (debug) printf(" > Final = %d", newval - carry * base);
            if (debug) printf("\n");
        }
        if (debug) printf("\n");

        a_i = 0;

        /* last digit(s) comes from final carry : */

        num_cur = ln_at(_out, a_i + b_i);
        if (num_cur < 0) num_cur = 0;
        num_new = num_cur + carry * base;

        /* Si la valeur que l'on doit indiquer est plus grande que la base,
        // alors nous allons avoir besoin d'ajouter un chiffre. */
        if (num_new >= base) {

            int doz = num_new / base;
            int dec = num_new - (num_new / base * base);

            /* On met le deuxieme chiffre d'abord, sinon l'index change si b_i vaut zéro. */
            _out->integer[b_i] = dec;

            if (b_i == 0) ln_prepend(_out, doz);
            else ln_set(_out, b_i - 1, doz);

        }
        else {
            ln_set(_out, b_i, num_cur + carry * base);
        }
    }
    /* Détermine le signe du résultat de la multiplication : */
    if (_b->negative != _a->negative &&
        (_b->negative || _a->negative)) {
        ln_negate(_out);
    }
}
/**
 * Where :
 *  _q : (out) quotien - could be NULL
 *  _n : number
 *  _d : divisor
 *  _r : (out) remainder - could be NULL
 */
/*
#define LN_LOGD2
*/
void ln_div(ln_t * _q, ln_t * _n, ln_t * _d, ln_t * _r) {

    ln_t current, div, tmp, tmp2;
    int it, stop, cur_idx_in_n = 0;

    if (ln_is_zero(_d)) {
        fprintf(stderr, "Division by zero.\n");
        return;
    }

    ln_init(&current);
    ln_init(&div);
    ln_init(&tmp);
    ln_init(&tmp2);

    while (cur_idx_in_n < _n->int_sz) {

        /* ETAPE 1. on recupere la partie de N plus grande que D. */

        it = 0;
        stop = 0;

        while (ln_cmp(&current, _d) == ln_Lesser) {

            if (_n->int_sz == cur_idx_in_n) { stop = 1; break; }

            ln_append(&current, ln_at(_n, cur_idx_in_n++));
            it++;

            #ifdef LN_LOGD2
            ln_show(&current, " (current)\n");
            #endif
        }

        #ifdef LN_LOGD2
        printf("iter = %d\n", it);
        #endif

        if (_q && it > 1 && _q->int_sz > 0) ln_append(_q, 0);

        ln_clear(&div);
        ln_append(&div, 1);

        while (1) {
            ln_clear(&tmp);
            ln_mul(&tmp, &div, _d);
            #ifdef LN_LOGD2
            ln_show(&div, " (div)\n");
            ln_show(_d, " (_d)\n");
            ln_show(&tmp, " = div * _d\n");
            #endif
            if (ln_cmp(&tmp, &current) == ln_Greater) {
            #ifdef LN_LOGD2
                ln_show(&div, " (div, greater)\n");
            #endif
                break;
            }
            ln_inc(&div);
        }
        ln_dec(&div);

        #ifdef LN_LOGD2
        ln_show(&div, " (div)\n");
        #endif


        if (!div.int_sz) ln_append(&div, 0);
        if (_q) ln_append(_q, ln_at(&div, 0));


        #ifdef LN_LOGD2
        ln_show(&div, " (append)\n");
        #endif

        ln_clear(&tmp);
        ln_mul(&tmp, &div, _d);
        ln_clear(&tmp2);
        ln_sub(&tmp2, &current, &tmp);
        ln_free(&current);
        ln_copy(&tmp2, &current);

        #ifdef LN_LOGD2
        ln_show(&current, " (current)\n");
        #endif

        if (stop) break;
    }

    if (_r) {
        ln_free(_r);
        ln_copy(&current, _r);
        if (_r->int_sz == 0) { ln_append(_r, 0); }
    }

    ln_free(&current);
    ln_free(&div);
    ln_free(&tmp);
    ln_free(&tmp2);
}

void ln_mod(ln_t * _out, ln_t * _a, ln_t * _b) {
    ln_div(NULL, _a, _b, _out);
}

/* x(20p+x) */
void x20ppx(ln_t * _out, ln_t * _p, int _x) {
    // printf(" ==> x20ppx(x=%d)\n", _x);

    ln_t tmp;
    ln_mul_str(_out, _p, "2", 1);

    // printf(" -> p*2="); ln_show(_out, "\n");

    ln_append(_out, _x);

    // printf(" -> p*2+x="); ln_show(_out, "\n");

    ln_copy(_out, &tmp);

    // printf(" -> temp="); ln_show(&tmp, "\n");

    ln_mul_int(_out, &tmp, _x);

    // printf("x20ppx : ");
    // printf("p="); ln_show(_p, "");
    // printf(", x=%d => ", _x);
    // ln_show(_out, "\n");

    ln_free(&tmp);
}

void ln_sqrt(ln_t * _out, ln_t * _n) {

    size_t it = 0;
    char va, vb;
    int x, cmp;

    ln_t remainder;
    ln_t current;
    ln_t part_of_root;
    ln_t tmp;
    ln_t tmp2;

    ln_init(&current);
    ln_init(&part_of_root);
    ln_init(&remainder);
    ln_init(&tmp);
    ln_init(&tmp2);

    ln_append(&part_of_root, 0);


    /* on doit obtenir les 2 chiffres les plus grands du nombre, mais de
        manière pair : ex: 123 => "01","23". */

    //ln_show(_n, " (number to square)\n");
    // printf("_n->int_sz = %lu\n", _n->int_sz);
    // printf("_n->int_sz/2 = %d\n", (int)(_n->int_sz/2.0));

    int sz2 = _n->int_sz / 2.0;
    int rem = _n->int_sz % 2;
    if (rem) sz2++;
    /* printf("sz2=%d (rem=%d)\n", sz2, rem); */
    it = sz2 * 2 - 1;

    while (1) {
        // ln_show(&part_of_root, " (part_of_root)\n");

        va = ln_last_at(_n, it);
        vb = ln_last_at(_n, it - 1);
        if (va < 0 && vb < 0) { break; } /* [todo] remove this break  */
        if (va < 0) va = 0;
        if (vb < 0) vb = 0;
        // printf("\t%d %d\n", va, vb);

        // remainder * 100 + va * 10 + vb
        ln_append(&remainder, va);
        ln_append(&remainder, vb);
        ln_trim(&remainder);
        // ln_show(&remainder, " (remainder)\n");

        x = 1;
        while (1) {
            ln_clear(&tmp);
            x20ppx(&tmp, &part_of_root, x);
            // ln_show(&tmp, " =x20ppx("); printf("%d)\n", x);
            cmp = ln_cmp(&tmp, &remainder);
            if (cmp == ln_Greater) { x--; break; }
            if (cmp == ln_Equal) { break; }
            x++;
        }
        ln_clear(&tmp);
        x20ppx(&tmp, &part_of_root, x);


        // printf(" => Found x = %d\n", x);
        ln_clear(&tmp2);
        ln_sub(&tmp2, &remainder, &tmp);

        ln_copy(&tmp2, &remainder);
        //ln_show(&remainder, " (new remainder)\n");

        ln_trim(&part_of_root);
        ln_append_int(&part_of_root, x);


        // ln_clear(&tmp);
        // x20ppx(&tmp, &part_of_root, 2);
        // ln_show(&tmp, " x(20p+x) w/ x=2\n");
        // ln_clear(&tmp);
        // x20ppx(&tmp, &part_of_root, 3);
        // ln_show(&tmp, " x(20p+x) w/ x=3\n");
        // break;

        it-=2;
    }

    ln_copy(&part_of_root, _out);

    // Freeing resources :
    ln_free(&current);
    ln_free(&remainder);
    ln_free(&part_of_root);
    ln_free(&tmp);
}