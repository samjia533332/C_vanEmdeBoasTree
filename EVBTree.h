//
//  EVBTree.h
//  C_Emode-Van-Boas-Tree
//
//  Created by Zhenyu Jia on 2018/11/26.
//  Copyright © 2018年 Zhenyu Jia. All rights reserved.
//

#ifndef EVBTree_h
#define EVBTree_h

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

typedef unsigned long long UUU;
typedef struct node vEBNode;

vEBNode*    createVEBTree       (UUU u);
void        destroyVEBTree      (vEBNode* root);
void        insertVEBTree       (vEBNode *root, UUU x);
void        deleteVEBTree       (vEBNode *root, UUU x);
UUU*        succVEBTree         (vEBNode *root, UUU x);
UUU*        predVEBTree         (vEBNode *root, UUU x);
bool        memberVEBTree       (vEBNode *root, UUU x);

#endif /* EVBTree_h */
