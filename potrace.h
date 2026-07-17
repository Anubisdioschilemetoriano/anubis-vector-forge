#ifndef _POTRACE_H_
#define _POTRACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* ----------------------------------------------------------------------
   Tipos de datos Potrace mínimo
   Adaptado del header público de Peter Selinger (potrace.sourceforge.net)
   Licencia GPLv2+ — ver LICENSE.txt en raíz del proyecto
   ---------------------------------------------------------------------- */

/* Un punto en el plano */
typedef struct {
    int x;
    int y;
} potrace_point_t;

/* Segmento de curva Bezier (tipo potrace_curve) */
typedef struct potrace_curve_s {
    int n;            /* número de puntos en esta curva */
    int* tag;         /* tag para cada punto: POTRACE_CORNER o POTRACE_CURVETO */
    potrace_point_t (*c)[3];  /* puntos de control [c[0], c[1], c[2]] */
} potrace_curve_t;

/* Un path completo (lista enlazada) */
typedef struct potrace_path_s {
    int area;               /* área del path (positiva = outer, negativa = hole) */
    int sign;               /* +1 o -1 */
    struct potrace_curve_s curve;  /* curva asociada */
    struct potrace_path_s* next;   /* siguiente path en la lista */
    struct potrace_path_s* childlist; /* hijos (holes) */
    struct potrace_path_s* sibling;  /* hermano */
} potrace_path_t;

/* Estado completo del bitmap Potrace */
typedef struct {
    int w, h;          /* dimensiones */
    int dy;            /* stride en palabras */
    unsigned long* bm; /* bitmap (1 bit por pixel, empaquetado) */
} potrace_bitmap_t;

/* Parámetros de trazado */
typedef struct {
    int turdsize;      /* ruido a suprimir (pixels) */
    int alphamax;      /* ángulo máximo de esquina */
    int opticurve;     /* curvatura óptica on/off */
    double scaling_factor;
} potrace_param_t;

/* Estado del trace */
typedef struct {
    int status;
    int progress;
    unsigned char* data;
    int len;
    potrace_path_t* plist; /* lista de paths trazados */
} potrace_state_t;

/* Tags para puntos de curva */
#define POTRACE_CURVETO 1
#define POTRACE_CORNER 2

/* Macros para manipulación de bitmap 1-bit (formato interno Potrace) */
#define BM_WORDSIZE ((int)sizeof(unsigned long))
#define BM_WORDBITS (8 * BM_WORDSIZE)
#define BM_HIBIT (((unsigned long)1) << (BM_WORDBITS - 1))
#define BM_ALLBITS (~(unsigned long)0)

#define bm_scanline(bm, y)  ((bm)->bm + (ptrdiff_t)(y) * (ptrdiff_t)(bm)->dy)
#define bm_index(bm, x, y)  (&bm_scanline(bm, y)[(x) / BM_WORDBITS])

#define BM_GET(bm, x, y) \
    ((bm_scanline(bm, y)[(x) / BM_WORDBITS] >> (BM_WORDBITS - 1 - ((x) % BM_WORDBITS))) & 1)
#define BM_SET(bm, x, y) \
    (bm_scanline(bm, y)[(x) / BM_WORDBITS] |= (BM_HIBIT >> ((x) % BM_WORDBITS)))
#define BM_CLR(bm, x, y) \
    (bm_scanline(bm, y)[(x) / BM_WORDBITS] &= ~(BM_HIBIT >> ((x) % BM_WORDBITS)))

/* API */
potrace_bitmap_t* potrace_bitmap_new(int w, int h);
void potrace_bitmap_free(potrace_bitmap_t* bm);

potrace_param_t* potrace_param_new();
void potrace_param_free(potrace_param_t* p);

potrace_state_t* potrace_trace(potrace_bitmap_t* bm, potrace_param_t* param);
void potrace_state_free(potrace_state_t* st);

#ifdef __cplusplus
}
#endif

#endif /* _POTRACE_H_ */
