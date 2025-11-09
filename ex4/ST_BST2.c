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

// 赤黒木への挿入(トップダウン)
link RBinsert(link h, Item item, int sw)
{
    Key v = key(item);
    // 葉(NIL)に到達：赤(red=1)の新規ノードを作って返す
    if (h == z) return NEW(item, z, z, 1, 1);

    // 子2つ(hl, hr)が赤 → 親hを赤に、子を黒にして 4-節点の分割を“上”に伝播
    // これにより、探索を深く進める前に 4-節点を分解しておき、葉での挿入が簡単になる
    if ((h->l->red) && (h->r->red)) {
        h->red = 1;
        h->l->red = 0;
        h->r->red = 0;
    }

    if (less(v, key(h->item))) {
        // 左部分木へ再帰的に挿入
        h->l = RBinsert(h->l, item, 0);
        // 親hが赤かつ左子h->lが赤で、かつこの部分木hが親の右子側のとき右回転
        // 親の赤赤を消して、赤は左に寄せる
        if (h->red && h->l->red && sw) h = rotR(h);
        // 左子と左孫がともに赤なら、右回転して3-節点/4節点の正しい向きに直す
        // 回転後，根hを黒，右子を赤にして色のバランス（2-3-4木の中央キー昇格に対応）
        if (h->l->red && h->l->l->red) {
            h = rotR(h);
            h->red = 0;
            h->r->red = 1;
        }
    } else {
        h->r = RBinsert(h->r, item, 1);
        // 親hが赤 かつ 右子h->rも赤 かつ “この部分木hは親の左子側（sw==0）” のとき左回転
        // → 親子の赤赤を解消して右への赤連鎖を断つ（左傾きへ是正）
        if (h->red && h->r->red && !sw) h = rotL(h);
        // 右子と右孫がともに赤（右に赤が2連続：RR）→ 左回転して“正しい向き”に直す
        // 回転後，根hを黒，左子を赤にして色のバランスを整える
        if (h->r->red && h->r->r->red) {
            h = rotL(h);
            h->red = 0;
            h->l->red = 1;
        }
    }

    // 部分木の節点数Nを更新
    h->N = h->l->N + h->r->N + 1;
    return h;   // 部分木の根を返す
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
