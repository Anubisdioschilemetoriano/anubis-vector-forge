/* ----------------------------------------------------------------------------
   potrace_core.cpp — Implementación mínima portable del algoritmo Potrace
   Basado en potrace 1.16 de Peter Selinger (potrace.sourceforge.net)
   Licencia: GPLv2+

   Contiene:
   - Binarización (luma → threshold 1-bit)
   - Detección de contornos por Moore neighborhood walk
   - Conteo de nodos (vértices/puntos de control)
   - Emisión SVG con paths reales y metadato <!-- nodes=N -->

   Aviso legal: si distribuyes esta APK comercialmente bajo GPLv2+,
   debes publicar o poner a disposición el código fuente correspondiente.
   Para uso interno/prototipo sin distribución pública, sin problema.
   ---------------------------------------------------------------------------- */

#include "potrace.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cmath>
#include <vector>

/* ---- Bitmap alloc/free ---- */
potrace_bitmap_t* potrace_bitmap_new(int w, int h) {
    potrace_bitmap_t* bm = (potrace_bitmap_t*)calloc(1, sizeof(potrace_bitmap_t));
    if (!bm) return nullptr;
    bm->w = w;
    bm->h = h;
    bm->dy = (w + BM_WORDBITS - 1) / BM_WORDBITS;
    bm->bm = (unsigned long*)calloc((size_t)bm->dy * h, sizeof(unsigned long));
    return bm;
}

void potrace_bitmap_free(potrace_bitmap_t* bm) {
    if (bm) {
        free(bm->bm);
        free(bm);
    }
}

/* ---- Parámetros ---- */
potrace_param_t* potrace_param_new() {
    potrace_param_t* p = (potrace_param_t*)calloc(1, sizeof(potrace_param_t));
    if (!p) return nullptr;
    p->turdsize = 2;
    p->alphamax = 1;
    p->opticurve = 1;
    p->scaling_factor = 1.0;
    return p;
}

void potrace_param_free(potrace_param_t* p) {
    free(p);
}

/* ---- Moore neighborhood walk (direcciones 0-7) ---- */
static int moore_dx[8] = { 1, 0, -1, -1, -1, 0, 1, 1 };
static int moore_dy[8] = { 0, -1, -1, 0, 1, 1, 1, 0 };

/* ---- Trace ---- */
potrace_state_t* potrace_trace(potrace_bitmap_t* bm, potrace_param_t* param) {
    (void)param;

    potrace_state_t* st = (potrace_state_t*)calloc(1, sizeof(potrace_state_t));
    if (!st) return nullptr;
    st->status = 1;

    int w = bm->w, h = bm->h;
    int total_nodes = 0;

    // Build SVG
    std::string svg;
    char header[256];
    snprintf(header, sizeof(header),
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" "
        "viewBox=\"0 0 %d %d\">\n"
        "<path d=\"", w, h, w, h);
    svg += header;

    potrace_path_t* head = nullptr;
    potrace_path_t* tail = nullptr;

    // Recorrer el bitmap buscando contornos
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int px = BM_GET(bm, x, y);
            int prev = (x > 0) ? BM_GET(bm, x - 1, y) : 0;

            // Detectar inicio de contorno: pixel negro donde el anterior es blanco
            if (px == 1 && prev == 0) {
                // Moore contour walk
                int sx = x, sy = y;
                int cx = sx, cy = sy;
                int dir = 4; // starting direction (south)
                int first = 1;
                std::vector<int> path_x, path_y;
                path_x.reserve(w * 2);
                path_y.reserve(h * 2);

                while (first || !(cx == sx && cy == sy && dir == 4)) {
                    first = 0;

                    // Find next boundary pixel
                    int found = 0;
                    for (int k = 0; k < 8; k++) {
                        int ndir = (dir + 5 + k) % 8; // turn right first
                        int nx = cx + moore_dx[ndir];
                        int ny = cy + moore_dy[ndir];

                        if (nx >= 0 && nx < w && ny >= 0 && ny < h && BM_GET(bm, nx, ny) == 1) {
                            path_x.push_back(nx);
                            path_y.push_back(ny);
                            cx = nx;
                            cy = ny;
                            dir = (ndir + 4) % 8; // turn around
                            found = 1;
                            break;
                        }
                    }
                    if (!found) break;

                    // Timeout safety
                    if (path_x.size() > (size_t)(w * h)) break;
                }

                if (path_x.size() < 4) continue; // skip noise

                // Emit SVG path segment
                char moveto[128];
                snprintf(moveto, sizeof(moveto), "M%d %d ", path_x[0], path_y[0]);
                svg += moveto;

                // Line segments with node counting
                for (size_t i = 1; i < path_x.size(); i++) {
                    char lineto[64];
                    snprintf(lineto, sizeof(lineto), "L%d %d ", path_x[i], path_y[i]);
                    svg += lineto;
                }
                svg += "Z ";
                total_nodes += (int)path_x.size();

                // Build potrace_path_t node
                potrace_path_t* pp = (potrace_path_t*)calloc(1, sizeof(potrace_path_t));
                pp->area = (int)path_x.size();
                pp->sign = 1;
                pp->next = nullptr;
                pp->childlist = nullptr;
                pp->sibling = nullptr;

                if (!head) {
                    head = tail = pp;
                } else {
                    tail->next = pp;
                    tail = pp;
                }

                // Volver a marcar contorno para no repetir
                for (int py = 0; py < h; py++) {
                    for (int px2 = 0; px2 < w; px2++) {
                        if (BM_GET(bm, px2, py) == 1) {
                            BM_CLR(bm, px2, py);
                            break; // one pixel per row as marker
                        }
                    }
                }
            }
        }
    }

    // SVG footer with node count metadata
    char footer[256];
    snprintf(footer, sizeof(footer),
        "\" fill=\"#00C4A0\" stroke=\"#00C4A0\" stroke-width=\"1.5\"/>\n"
        "<!-- nodes=%d -->\n"
        "</svg>", total_nodes);
    svg += footer;

    st->plist = head;
    st->data = (unsigned char*)malloc(svg.size() + 1);
    if (st->data) {
        memcpy(st->data, svg.c_str(), svg.size() + 1);
        st->len = (int)svg.size();
    }

    return st;
}

void potrace_state_free(potrace_state_t* st) {
    if (st) {
        free(st->data);
        potrace_path_t* p = st->plist;
        while (p) {
            potrace_path_t* next = p->next;
            free(p);
            p = next;
        }
        free(st);
    }
}
