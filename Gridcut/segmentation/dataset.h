#ifndef DATASET_H_
#define DATASET_H_

struct Instance
{
  const char* name;
  int         count;
  const char* filename;
};

const Instance instances[] = {

  { "flower",           3, "%s/flower/flower%d.mfi" },
  { "person",           3, "%s/person/person%d.mfi" },
  { "sponge",           3, "%s/sponge/sponge%d.mfi" },

};

#endif
