#include <stdio.h>
#include <stdlib.h>
#include "Item.h"
#include "ST.h"

// メイン関数
int main(int argc, char *argv[])
{
  // maxN: 挿入する要素数, sw: 挿入方法のオプション
  int N, M, maxN = atoi(argv[1]), sw = atoi(argv[2]);
  Key v; Item item;

  STinit(maxN);      // 記号表（木）の初期化
  srand(1);          // 乱数の初期化（ランダム挿入用）

  // maxN回繰り返し、swに応じて値を決定して挿入
  for (M = 0, N = 0; N < maxN; N++)
    {
      // sw==1: ランダムな値, sw==2: 昇順, それ以外: 標準入力から値取得
      if (sw == 1) v = ITEMrand();      // ランダム値
      else if (sw == 2) v = N+1;        // 昇順値
      else if (ITEMscan(&v) == EOF) break; // 標準入力

      item = STsearch(v);               // 既に木に存在するか検索
      if (item.key != NULLitem.key) continue; // 重複ならスキップ

      key(item) = v;                    // Itemに値をセット
      STinsert(item);                   // 木に挿入
      M++;                              // 挿入数カウント
    }

  STsort(ITEMshow); printf("\n");       // 木の要素を昇順で表示
  printf("%d keys ", N);                // 挿入したキー数
  printf("%d distinct keys\n", STcount()); // 重複しないキー数（節点数）

  printf("\nTree structure:\n");
  STshowAll();                          // 木の形状（構造）を表示

  return 0;
}
