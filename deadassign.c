/*
***********************************************************************
  DEADASSIGN.C : IMPLEMENT THE DEAD CODE ELIMINATION OPTIMIZATION HERE
************************************************************************
*/

#include "deadassign.h"

int change;
refVar *last, *head;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE HEAD AND LAST POINTERS FOR THE REFERENCED 
  VARIABLE LIST.
************************************************************************
*/

void init()
{ 
    head = NULL;
    last = head;
}

/*
***********************************************************************
  FUNCTION TO FREE THE REFERENCED VARIABLE LIST
************************************************************************
*/

void FreeList()
{
   refVar* tmp;
   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO IDENTIFY IF A VARIABLE'S REFERENCE IS ALREADY TRACKED
************************************************************************
*/
bool VarExists(char* name) {
   refVar *node;
   node = head;
   while (node != NULL) {
       if (!strcmp(name, node->name)) {
           return true;
       }
       node = node->next;
    }
    return false;
}

/*
***********************************************************************
  FUNCTION TO ADD A REFERENCE TO THE REFERENCE LIST
************************************************************************
*/
void UpdateRefVarList(char* name) {
    refVar* node = malloc(sizeof(refVar));
    if (node == NULL) return;
    node->name = name;
    node->next = NULL;
    if (head == NULL) {
        last = node;
        head = node;
    }
    else {
        last->next = node;
        last = node;
    }
}

/*
****************************************************************************
  FUNCTION TO PRINT OUT THE LIST TO SEE ALL VARIABLES THAT ARE USED/REFERRED
  AFTER THEIR ASSIGNMENT. YOU CAN USE THIS FOR DEBUGGING PURPOSES OR TO CHECK
  IF YOUR LIST IS GETTING UPDATED CORRECTLY
******************************************************************************
*/
void PrintRefVarList() {
    refVar *node;
    node = head;
    if (node == NULL) {
        printf("\nList is empty"); 
        return;
    }
    while (node != NULL) {
        printf("\t %s", node->name);
        node = node->next;
    }
}

/*
***********************************************************************
  FUNCTION TO UPDATE THE REFERENCE LIST WHEN A VARIABLE IS REFERENCED 
  IF NOT DONE SO ALREADY.
************************************************************************
*/
void UpdateRef(Node* node) {
      if (node->right != NULL && node->right->exprCode == VARIABLE) {
          if (!VarExists(node->right->name)) {
              UpdateRefVarList(node->right->name);
          }
      }
      if (node->left != NULL && node->left->exprCode == VARIABLE) {
          if (!VarExists(node->left->name)) {
              UpdateRefVarList(node->left->name);
          }
      }
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/


/*
********************************************************************
  THIS FUNCTION IS MEANT TO TRACK THE REFERENCES OF EACH VARIABLE
  TO HELP DETERMINE IF IT WAS USED OR NOT LATER
********************************************************************
*/

void TrackRef(Node* funcNode) {
  NodeList* statements = funcNode->statements;
  Node *node;
  change = 0;
  while (statements != NULL) {
    node = statements->node;
    // Always keep track of return variable (if applicable)
    if (node->stmtCode == RETURN && node->left->exprCode == VARIABLE) {
      if (!VarExists(node->left->name)) {
        UpdateRefVarList(node->left->name);
        change = 1;
      }
    }
    // Update assignment variables if necessary
    else {
      if (VarExists(node->name)) {
        // Case 1: Assignment to variable
        if (node->right->exprCode == VARIABLE && !VarExists(node->right->name)) {
          UpdateRefVarList(node->right->name);
          change = 1;
        }
        // Case 2: Assignment to Unary operation
        else if (node->right->opCode == NEGATE) {
          if (node->right->left->exprCode == VARIABLE && !VarExists(node->right->left->name)) {
            UpdateRefVarList(node->right->left->name);
            change = 1;
          }
        }
        // Case 3: Assignment to function call
        else if (node->right->opCode == FUNCTIONCALL) {
          NodeList* arg = node->right->arguments;
          while (arg != NULL) {
            if (arg->node->exprCode == VARIABLE && !VarExists(arg->node->name)) {
              UpdateRefVarList(arg->node->name);
              change = 1;
            }
            arg = arg->next;
          }
        }
        // Case 4: Assignment to Binary operation
        else if (node->right->exprCode = OPERATION) {
          if (node->right->left->exprCode == VARIABLE && !VarExists(node->right->left->name)) {
            UpdateRefVarList(node->right->left->name);
            change = 1;
          }
          if (node->right->right->exprCode == VARIABLE && !VarExists(node->right->right->name)) {
            UpdateRefVarList(node->right->right->name);
            change = 1;
          }
        }
      }
    }
      
    statements = statements->next;
  }
}

/*
***************************************************************
  THIS FUNCTION IS MEANT TO DO THE ACTUAL DEADCODE REMOVAL
  BASED ON THE INFORMATION OF TRACKED REFERENCES
****************************************************************
*/
NodeList* RemoveDead(NodeList* statements) {
    refVar* varNode;
    NodeList *prev, *tmp, *first;
    Node* node;
    first = statements;
    prev = NULL;
    while (statements != NULL) {
      node = statements->node;
      if (node->stmtCode == ASSIGN && !VarExists(node->name)) {
        if (prev == NULL) {
          first = statements->next;
        }
        else {
          prev->next = statements->next;
        }
        tmp = statements->next;
        FreeAssignment(node);
        free(statements);
        statements = tmp;
        change = 1;
      }
      else {
        prev = statements;
        statements = statements->next;
      }
    }
  
   return first;
}

/*
********************************************************************
  THIS FUNCTION SHOULD ENSURE THAT THE DEAD CODE REMOVAL PROCESS
  OCCURS CORRECTLY FOR ALL THE FUNCTIONS IN THE PROGRAM
********************************************************************
*/
bool DeadAssign(NodeList* funcdecls) {
   bool madeChange = false;
   while (funcdecls != NULL) {
    // Create reference list
    init();
    change = 1;
    while (change) {
      TrackRef(funcdecls->node);
    }

    // Remove dead assignments
    NodeList* first = RemoveDead(funcdecls->node->statements);
    funcdecls->node->statements = first;
    
    // Reset reference list and move onto next function
    FreeList();
    funcdecls = funcdecls->next;
  }
  if (change == 1) madeChange=true;
  return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
 
