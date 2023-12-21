/*
********************************************************************************
  CONSTPROP.C : IMPLEMENT THE DOWNSTREAM CONSTANT PROPOGATION OPTIMIZATION HERE
*********************************************************************************
*/

#include "constprop.h"

refConst *lastNode, *headNode;
/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO FREE THE CONSTANTS-ASSOCIATED VARIABLES LIST
************************************************************************
*/
void FreeConstList()
{
   refConst* tmp;
   while (headNode != NULL)
    {
       tmp = headNode;
       headNode = headNode->next;
       free(tmp);
    }

}

/*
*************************************************************************
  FUNCTION TO ADD A CONSTANT VALUE AND THE ASSOCIATED VARIABLE TO THE LIST
**************************************************************************
*/
void UpdateConstList(char* name, long val) {
    refConst* node = malloc(sizeof(refConst));
    if (node == NULL) return;
    node->name = name;
    node->val = val;
    node->next = NULL;
    if (headNode == NULL) {
        lastNode = node;
        headNode = node;
    }
    else {
        lastNode->next = node;
        lastNode = node;
    }
}

/*
*****************************************************************************
  FUNCTION TO LOOKUP IF A CONSTANT ASSOCIATED VARIABLE IS ALREADY IN THE LIST
******************************************************************************
*/
refConst* LookupConstList(char* name) {
    refConst *node;
    node = headNode; 
    while (node != NULL){
        if (!strcmp(name, node->name))
            return node;
        node = node->next;
    }
    return NULL;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/


/*
************************************************************************************
  THIS FUNCTION IS MEANT TO UPDATE THE CONSTANT LIST WITH THE ASSOCIATED VARIABLE
  AND CONSTANT VALUE WHEN ONE IS SEEN. IT SHOULD ALSO PROPOGATE THE CONSTANTS WHEN 
  WHEN APPLICABLE. YOU CAN ADD A NEW FUNCTION IF YOU WISH TO MODULARIZE BETTER.
*************************************************************************************
*/
void TrackConst(NodeList* statements) {
        Node* node;
        refConst* lookup;
        while (statements != NULL) {
        node = statements->node;
        // Propogate constant in return statement
        if (node->stmtCode == RETURN) {
          if (node->left->exprCode == VARIABLE) {
            lookup = LookupConstList(node->left->name);
            if (lookup != NULL) {
              FreeVariable(node->left);
              node->left = CreateNumber(lookup->val);
              madeChange = true;
            }
          }
        }
        // Propogate in assignment statement
        else {
          Node* stmtNodeRight = node->right;
          // Propogate in the arguments of function calls
          if (stmtNodeRight->opCode == FUNCTIONCALL) {
            NodeList* arg = stmtNodeRight->arguments;
            while (arg != NULL) {
              if (arg->node->exprCode == VARIABLE) {
                lookup = LookupConstList(arg->node->name);
                if (lookup != NULL) {
                  FreeVariable(arg->node);
                  arg->node = CreateNumber(lookup->val);
                  madeChange = true;
                }
              }
              arg = arg->next;
            }
          }
          // Propogate in Unary operations
          else if (stmtNodeRight->opCode == NEGATE && stmtNodeRight->left->exprCode == VARIABLE) {
            lookup = LookupConstList(stmtNodeRight->left->name);
            if (lookup != NULL) {
              FreeVariable(stmtNodeRight->left);
              stmtNodeRight->left = CreateNumber(lookup->val);
              madeChange = true;
            }
          }
          // Propogate in single variable assignments
          else if (stmtNodeRight->exprCode == VARIABLE) {
            lookup = LookupConstList(stmtNodeRight->name);
            if (lookup != NULL) {
              FreeVariable(stmtNodeRight);
              node->right = CreateNumber(lookup->val);
              madeChange = true;
            }
          }
          // Update lookup list if the assignment's left is a constant
          else if (stmtNodeRight->exprCode == CONSTANT) {
            lookup = LookupConstList(node->name);
            if (lookup == NULL) {
              UpdateConstList(node->name, stmtNodeRight->value);
            }
          }
          // Propogate in Binary operations
          else {
            // Left side
            if (stmtNodeRight->left->exprCode == VARIABLE) {
              lookup = LookupConstList(stmtNodeRight->left->name);
              if (lookup != NULL) {
                FreeVariable(stmtNodeRight->left);
                stmtNodeRight->left = CreateNumber(lookup->val);
                madeChange = true;
              }
            }
            // Right side
            if (stmtNodeRight->right->exprCode == VARIABLE) {
              lookup = LookupConstList(stmtNodeRight->right->name);
              if (lookup != NULL) {
                FreeVariable(stmtNodeRight->right);
                stmtNodeRight->right = CreateNumber(lookup->val);
                madeChange = true;
              }
            }
          }

        }

        statements = statements->next;
    }
}


bool ConstProp(NodeList* funcdecls) {
  madeChange = false;
    while (funcdecls != NULL){
        TrackConst(funcdecls->node->statements);
        FreeConstList();
        funcdecls = funcdecls->next;
    }
  return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
