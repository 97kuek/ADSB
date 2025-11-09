// 第5回アルゴリズムとデータ構造B 演習課題

#include <stdlib.h>
#include <stdio.h>
#include "Item.h"

// B木の次数（オーダー）を定義。各ページ（ノード）が持てる最大の子の数。
#define M 5
// 各ページが持てる最大の項目数（キーと参照のペア）
#define ws (M+1)

// B木のページ（ノード）へのポインタ型を定義
typedef struct STnode* link;

// ページ内に格納される項目（エントリ）の構造体
typedef struct
{
    Key key; // 探索の際に使われるキー
    union {
        link next; // 内部節点の場合：次のレベルのページへのポインタ
        Item item; // 葉の場合：実際のデータ項目
    } ref;
}
    entry;

// B木のページ（ノード）の構造体
struct STnode{entry b[ws]; int m;};

// --- B木全体を管理するための静的グローバル変数 ---
static link head;       // 木の根（ルート）ページへのポインタ
static int H;           // 木の現在の高さ
static int N;           // 木に格納されている総項目数
static int page_count;  // 木全体の総ページ（ノード）数

// --- STshowAll（レベル順表示）で使うためのシンプルなキュー実装 ---
#define MAX_QUEUE_SIZE 1000
static link queue[MAX_QUEUE_SIZE];
static int queue_front = 0;
static int queue_rear = 0;

// キューをリセットする
void reset_queue() {
    queue_front = 0;
    queue_rear = 0;
}

// キューにページ（ノード）を追加する
void enqueue(link node) {
    if (queue_rear < MAX_QUEUE_SIZE) {
        queue[queue_rear++] = node;
    }
}

// キューからページ（ノード）を取り出す
link dequeue() {
    if (queue_front < queue_rear) {
        return queue[queue_front++];
    }
    return NULL;
}

// キューが空かどうかをチェックする
int is_queue_empty() {
    return queue_front == queue_rear;
}
// --- キュー実装ここまで ---

// 新しい空のページ（ノード）を作成し、そのポインタを返す
link NEW()
{
    link x = malloc(sizeof *x); // メモリを確保
    x->m = 0;                   // 項目数を0に初期化
    page_count++;               // 総ページ数をインクリメント
    return x;
}

// B木全体を初期化する
void STinit(int maxN)
{
    page_count = 0; // 総ページ数をリセット
    head = NEW();   // 根（ルート）となる最初のページを作成
    H = 0;          // 木の高さを0に初期化
    N = 0;          // 総項目数を0に初期化
}

// --- 木の情報を取得する各種関数 ---
int STorder(void) { return M; }          // B木の次数Mを返す
int STheight(void) { return H; }         // 木の高さを返す
int STpages(void) { return page_count; } // 総ページ数を返す
int STcount(void) { return N; }          // 総項目数を返す

// ページhを2つに分割し、新しく作成したページのポインタを返す
link split(link h)
{
    int j;
    link t = NEW(); // 新しいページを作成
    // 元のページhの後半(M/2)個の項目を、新しいページtにコピーする
    for(j = 0; j < ws/2; j++)
	t    t->b[j] = h->b[ws/2+j];
    // 元のページと新しいページの項目数を更新する
    h->m = ws/2;
    t->m = ws/2;
    return t;
}

// ページhを根とする高さHの部分木から、キーvを再帰的に探索する
Item searchR(link h, Key v, int H)
{
    int j;
    // ベースケース：葉（高さ0）に到達した場合
    if(H == 0)
	for(j = 0; j < h->m; j++)
	    if(eq(v, h->b[j].key))
		return h->b[j].ref.item; // キーが見つかればItemを返す
    // 再帰ステップ：内部節点の場合
    if(H != 0)
	for(j = 0;j < h->m; j++)
	    if((j+1 == h->m)||less(v, h->b[j+1].key))
		return searchR(h->b[j].ref.next, v, H-1); // 再帰呼び出し
    return NULLitem; // キーが見つからなければNULLitemを返す
}

// B木全体からキーvを探索する公開インタフェース
Item STsearch(Key v)
{ return searchR(head, v, H); }

// ページhを根とする高さHの部分木に、項目itemを再帰的に挿入する
// ページ分割が発生した場合、新しく作られたページのポインタを返す
link insertR(link h, Item item, int H)
{
    int i, j; Key v = key(item); entry x; link t, u;
    x.key = v;
    x.ref.item = item;

    // ベースケース：葉（高さ0）に到達した場合
    if(H == 0)
	for(j = 0;j < h->m; j++)
	    if(less(v, h->b[j].key)) break;
    // 再帰ステップ：内部節点の場合
    if(H != 0)
	for(j = 0;j < h->m; j++)
	    if((j+1 == h->m)||less(v, h->b[j+1].key))
		{
		    t = h->b[j++].ref.next;
		    u = insertR(t, item, H-1); // 再帰呼び出し
		    if(u == NULL) return NULL; // 子で分割が起きなければ終了
		    // 子で分割が起きた場合、新しい項目（分割されたページの先頭キー）を現在のページに挿入
		    x.key = u->b[0].key; x.ref.next = u;
		    break;
		}
    // 見つけた位置jに新しい項目xを挿入
    for(i = (h->m)++; i > j; i--)
	h->b[i] = h->b[i-1];
    h->b[j] = x;

    // ページが満杯でなければ終了。満杯なら分割して新しいページを返す
    if(h->m < ws) return NULL; else return split(h);    
}

// B木に項目itemを挿入する公開インタフェース
void STinsert(Item item)
{
    // 再帰的な挿入処理を呼び出す
    link u = insertR(head, item, H);
    N++; // 総項目数をインクリメント
    if(u == NULL) return; // 分割がなければ終了

    // ルートが分割された場合の処理
    link t = NEW(); // 新しいルートページを作成
    t->m = 2;
    // 古いルートと、分割で新しくできたページを新しいルートの子にする
    t->b[0].key = head->b[0].key;
    t->b[0].ref.next = head;
    t->b[1].key = u->b[0].key;
    t->b[1].ref.next = u;
    head = t; H++;      // 木の高さをインクリメント
}

// B木の構造をレベル（階層）ごとに表示する
void STshowAll(void) {
    if (head == NULL || N == 0) {
        printf("Tree is empty\n");
        return;
    }

    reset_queue();
    enqueue(head);
    enqueue(NULL); // レベルの終わりを示すマーカー

    int current_level_H = H;

    while (!is_queue_empty()) {
        link h = dequeue();

        // レベルマーカー(NULL)を検出した場合
        if (h == NULL) {
            printf("\n"); // 改行して次のレベルへ
            if (is_queue_empty()) break; // 全レベル表示完了
            enqueue(NULL); // 次のレベルのマーカーを追加
            current_level_H--;
            continue;
        }

        // 現在のページのキーを表示
        printf("|");
        int j;
        if (current_level_H == 0) { // 葉レベルの場合
            for (j = 0; j < h->m; j++) {
                printf("%d", key(h->b[j].ref.item));
                if (j < h->m - 1) printf(", ");
            }
        } else { // 内部節点の場合
            for (j = 0; j < h->m; j++) {
                printf("%d", h->b[j].key);
                if (j < h->m - 1) printf(", ");
            }
        }
        printf("| ");

        // 内部節点であれば、子ページをキューに追加
        if (current_level_H > 0) {
            for (j = 0; j < h->m; j++) {
                enqueue(h->b[j].ref.next);
            }
        }
    }
}

// --- 互換性のために残されているが、現在使われていない関数 ---
void sortR(link h, int H, void (*visit)(Item)) { }
void STsort(void (*visit)(Item)) { }
void STdelete(Item item) { }
Item STselect(int k) { return NULLitem; }