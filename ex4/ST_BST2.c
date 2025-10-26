// ADSB 演習4-2: 赤黒木の実装と表示

#include <stdlib.h>
#include <stdio.h>
#include "Item.h"
#include "ST.h"

typedef struct STnode* link;
struct STnode {
    Item item;                                          // ノードが保持するItem
    link l, r;                                          // 左右のの部分木への根へのポインタ
    int  N;                                             // ノードを根とする部分木の節点数
    int red;                                            // 親へのリンクの色
};
static link head, z;                                    // head: 全体の根, z: ダミー節点

// 節点をインスタンス化
link NEW(Item item, link l, link r, int N, int red)
{
    link x = malloc(sizeof *x);
    x->item = item; x->l = l; x->r = r; x->N = N; x->red = red;
    return x;
}

// ダミー節点を作り、headとzがそれを指すように初期化
void STinit(int maxN)
{
    head = (z = NEW(NULLitem, 0, 0, 0, 0));
}

// 節点の総数のカウント機能
// head の N を返せばよい
int STcount(void)
{
    return head->N;
}

// 探索の実装(再帰呼び出し)
Item searchR(link h, Key v)
{
    Key t = key(h->item);                               // 現在ノードのキー
    if (h == z) return NULLitem;                        // hがダミー節点なら"探索不成功"
    if eq(v, t) return h->item;                         // 一致すれば成功
    if less(v, t) return searchR(h->l, v);              // v < t なら左の部分木へ
    else return searchR(h->r, v);                       // v > t なら右の部分木へ
}

// 探索
Item STsearch(Key v)
{
    return searchR(head, v);
}

// 詳しくは講義ビデオPart4参照↓

// 右回転の実装
link rotR(link h)
{
    link x = h->l;                                      // xを軸に右回転
    h->l = x->r;                                        // hの右リンクはそのまま、hの左リンクはxの右のリンクに
    x->r = h;                                           // xの右リンクはhに、xの左リンクはそのまま

    x->N = h->N;                                        // xの節点数Nは旧hと同じ
    h->N = h->l->N + h->r->N + 1;
    return x;
}

// 左回転の実装
link rotL(link h)
{
    link x = h->r;                                      // xを軸に左回転
    h->r = x->l;                                        // hの左リンクはそのまま、hの右リンクはxの左のリンクに
    x->l = h;                                           // xの左リンクはhに、xの右リンクはそのまま

    x->N = h->N;                                        // xの節点数Nは旧hと同じ
    h->N = h->l->N + h->r->N + 1;
    return x;
}

link RBinsert(link h, Item item, int sw)
{
    Key v = key(item);
    if (h == z) return NEW(item, z, z, 1, 1);

    if ((h->l->red) && (h->r->red)) {
        h->red = 1;
        h->l->red = 0;
        h->r->red = 0;
    }

    if (less(v, key(h->item))) {
        h->l = RBinsert(h->l, item, 0);
        if (h->red && h->l->red && sw) h = rotR(h);
        if (h->l->red && h->l->l->red) {
            h = rotR(h);
            h->red = 0;
            h->r->red = 1;
        }
    } else {
        h->r = RBinsert(h->r, item, 1);
        if (h->red && h->r->red && !sw) h = rotL(h);
        if (h->r->red && h->r->r->red) {
            h = rotL(h);
            h->red = 0;
            h->l->red = 1;
        }
    }
    h->N = h->l->N + h->r->N + 1;
    return h;
}

void STinsert(Item item)
{
    head = RBinsert(head, item, 0);
    head->red = 0;
}

// 中央順走査での整列を出力する関数
void sortR(link h, void (*visit)(Item))
{
  if (h == z) return;     // ダミー節点なら終了
  sortR(h->l, visit);     // 左部分木を先に操作
  visit(h->item);         // 自ノード処理
  sortR(h->r, visit);     // 右部分木を後で操作
}

// 外部API：全体を昇順に visit で処理
void STsort(void (*visit)(Item))
{
    sortR(head, visit);
}

// 木の形状の表示機能
void STshow(link h, int l, int black_count)
{
    int i;
    if (h->red == 0) black_count++; // 黒い節点ならカウントを増やす

    if (h == z) {                                       // ダミー節点なら
        for (i = 0; i < l; i++) printf("\t");           // レベル分タブを出力
        printf("(-1,0,B, black:%d)\n", black_count);   // (-1,0,B)と黒節点数を出力
        return;
    }
    STshow(h->r, l + 1, black_count);                                // 右部分木を先に表示
    for (i = 0; i < l; i++) printf("\t");               // レベル分タブを出力
    printf("(%d,%d,%c)\n", key(h->item), h->N, h->red ? 'R' : 'B'); // (キー,節点数,色)を出力
    STshow(h->l, l + 1, black_count);                                // 左部分木を後で表示
}

// 根から全体を表示
void STshowAll()
{
    STshow(head, 0, 0);
}
