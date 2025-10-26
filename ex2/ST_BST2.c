// ADSB 演習2-2: 節点の総数のカウント機能・木の形状の表示機能の実装

#include <stdlib.h>
#include <stdio.h>
#include "Item.h"
#include "ST.h"

typedef struct STnode* link;
struct STnode {
    Item item;                                          // ノードが保持するItem
    link l, r;                                          // 左右のの部分木への根へのポインタ
    int  N;                                             // ノードを根とする部分木の節点数
};
static link head, z;                                    // head: 全体の根, z: ダミー節点

// 節点をインスタンス化
link NEW(Item item, link l, link r, int N)
{
    link x = malloc(sizeof *x);
    x->item = item; x->l = l; x->r = r; x->N = N;
    return x;
}

// ダミー節点を作り、headとzがそれを指すように初期化
void STinit(int maxN)
{
    head = (z = NEW(NULLitem, 0, 0, 0));
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

// 根への挿入の実装
link insertT(link h, Item item)
{
    Key v = key(item);                                  // 挿入アイテムのキー
    if (h == z) return NEW(item, z, z, 1);              // itemを木の底に挿入

    if (less(v, key(h->item)))                          // 挿入したキーのほうが小さい場合
    {
        h->l = insertT(h->l, item);                     // 左部分木へ再帰挿入
        h->N = h->l->N + h->r->N + 1;                   // h のサイズを更新
        h = rotR(h);                                    // 右回転
    }
    else                                                // 挿入したキーのほうが大きい場合
    {
        h->r = insertT(h->r, item);                     // 右部分木へ再帰挿入
        h->N = h->l->N + h->r->N + 1;                   // h のサイズ更新
        h = rotL(h);                                    // 左回転
    }
    return h;                                           // 回転後の部分木根を返す
}

void STinsert(Item item)
{
    head = insertT(head, item);
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
void STshow(link h, int l)
{
    int i;
    if (h == z) {                                       // ダミー節点なら
        for (i = 0; i < l; i++) printf("\t");           // レベル分タブを出力
        printf("(-1,0)\n");                             // (-1,0)を出力
        return;
    }
    STshow(h->r, l + 1);                                // 右部分木を先に表示
    for (i = 0; i < l; i++) printf("\t");               // レベル分タブを出力
    printf("(%d,%d)\n", key(h->item), h->N);            // (キー,節点数)を出力
    STshow(h->l, l + 1);                                // 左部分木を後で表示
}

// 根から全体を表示
void STshowAll()
{
    STshow(head, 0);
}
