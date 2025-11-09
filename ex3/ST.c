// ADSB 演習3-2

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

// スプレイ操作
// 1. 根から順に新たな節点の挿入する位置を探索
// 2. 2回連続して左向きor右向きに曲がる場合は2回右回転or左回転を行う
// 3. 左向きと右向きに曲がる場合は左回転と右回転を行う

link splay (link h, Item item)
{
  Key v = key (item);
  // 木が空の場合
  if (h == z)
    return NEW (item, z, z, 1);

  // ある部分木の根hの左に挿入されるとき
  if (less (v, key (h->item)))
    {
      if (h->l == z)
	return NEW (item, z, h, h->N + 1);  
      if (less (v, key (h->l->item)))     // さらに左に挿入されるとき
	{
	  h->l->l = splay (h->l->l, item);      // 根の左の左に，挿入する節点を根とした部分木を持ってきて，
	  h = rotR (h);                         // 右回転で根を上げる
	}
      else if (less(key(h->l->item), v))  // 根の左の右に挿入されるとき
	{
	  h->l->r = splay (h->l->r, item);
	  h->l = rotL (h->l);                   // 根の左の節点で左回転
	}
      return rotR (h);                    // 根でもう一回右回転
    }

  // ある部分木の根hの右に挿入されるとき
  else if (less(key(h->item), v))
    {
      if (h->r == z)
	return NEW (item, h, z, h->N + 1);
      if (less (key (h->r->item), v))
	{
	  h->r->r = splay (h->r->r, item);
	  h = rotL (h);
	}
      else if (less(v, key(h->r->item)))
	{
	  h->r->l = splay (h->r->l, item);
	  h->r = rotR (h->r);
	}
      return rotL (h);
    }
    else {
        return h;
    }
}


void STinsert(Item item)
{
    head = splay(head, item);                                               // スプレイ操作を実行して，挿入したitemを含む部分木を根に持ってくる。
    head->N = (head->l ? head->l->N : 0) + (head->r ? head->r->N : 0) + 1;  // 根の節点数Nを更新，左右の部分木のノード数を合計し，自分自身の+1を足す
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
