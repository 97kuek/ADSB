#define hl  h->l
#define hr  h->r
#define hll h->l->l
#define hlr h->l->r
#define hrl h->r->l
#define hrr h->r->r

struct STnode
{
  Item item;
  link l, r;
  int N;
  int red;
};
link
NEW (Item item, link l, link r, int N, int red)
{
  link x = malloc (sizeof *x);
  x->item = item;
  x->l = l;
  x->r = r;
  x->N = N;
  x->red = red;
  return x;
}

void
STinit ()
{
  head = (z = NEW (NULLitem, 0, 0, 0, 0));
}

link
RBinsert (link h, Item item, int sw)
{
  Key v = key (item);
  if (h == z)
    return NEW (item, z, z, 1, 1);
  if ((hl->red) && (hr->red))
    {
      h->red = 1;
      hl->red = 0;
      hr->red = 0;
    }
  if (less (v, key (h->item)))
    {
      hl = RBinsert (hl, item, 0);
      if (h->red && hl->red && sw)
	h = rotR (h);
      if (hl->red && hll->red)
	{
	  h = rotR (h);
	  h->red = 0;
	  hr->red = 1;
	}
    }
  else
    {
      hr = RBinsert (hr, item, 1);
      if (h->red && hr->red && !sw)
	h = rotL (h);
      if (hr->red && hrr->red)
	{
	  h = rotL (h);
	  h->red = 0;
	  hl->red = 1;
	}
    }
  return h;
}

void
STinsert (Item item)
{
  head = RBinsert (head, item, 0);
  head->red = 0;
}
