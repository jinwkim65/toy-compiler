/*
***********************************************************************
  CODEGEN.C : IMPLEMENT CODE GENERATION HERE
************************************************************************
*/
#include "codegen.h"

int argCounter;
int lastUsedOffset;
char lastOffsetUsed[100];
FILE *fptr;
regInfo *regList, *regHead, *regLast;
varStoreInfo *varList, *varHead, *varLast;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE THE ASSEMBLY FILE WITH FUNCTION DETAILS
************************************************************************
*/
void InitAsm(char* funcName) {
    fprintf(fptr, "\n.globl %s", funcName);
    fprintf(fptr, "\n%s:", funcName); 

    // Init stack and base ptr
    fprintf(fptr, "\npushq %%rbp");  
    fprintf(fptr, "\nmovq %%rsp, %%rbp"); 
}

/*
***************************************************************************
   FUNCTION TO WRITE THE RETURNING CODE OF A FUNCTION IN THE ASSEMBLY FILE
****************************************************************************
*/
void RetAsm() {
    fprintf(fptr,"\npopq  %%rbp");
    fprintf(fptr, "\nretq\n");
} 

/*
***************************************************************************
  FUNCTION TO CONVERT OFFSET FROM LONG TO CHAR STRING 
****************************************************************************
*/
void LongToCharOffset() {
     lastUsedOffset = lastUsedOffset - 8;
     snprintf(lastOffsetUsed, 100,"%d", lastUsedOffset);
     strcat(lastOffsetUsed,"(%rbp)");
}

/*
***************************************************************************
  FUNCTION TO CONVERT CONSTANT VALUE TO CHAR STRING
****************************************************************************
*/
void ProcessConstant(Node* opNode) {
     char value[10];
     LongToCharOffset();
     snprintf(value, 10,"%ld", opNode->value);
     char str[100];
     snprintf(str, 100,"%d", lastUsedOffset);
     strcat(str,"(%rbp)");
     AddVarInfo("", str, opNode->value, true);
     fprintf(fptr, "\nmovq  $%s, %s", value, str);
}

/*
***************************************************************************
  FUNCTION TO SAVE VALUE IN ACCUMULATOR (RAX)
****************************************************************************
*/
void SaveValInRax(char* name) {
    char *tempReg;
    tempReg = GetNextAvailReg(true);
    if (!(strcmp(tempReg, "NoReg"))) {
        LongToCharOffset();
        fprintf(fptr, "\n movq %%rax, %s", lastOffsetUsed);
        UpdateVarInfo(name, lastOffsetUsed, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
    else {
        fprintf(fptr, "\nmovq %%rax, %s", tempReg);
        UpdateRegInfo(tempReg, 0);
        UpdateVarInfo(name, tempReg, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
}



/*
***********************************************************************
  FUNCTION TO ADD VARIABLE INFORMATION TO THE VARIABLE INFO LIST
************************************************************************
*/
void AddVarInfo(char* varName, char* location, long val, bool isConst) {
   varStoreInfo* node = malloc(sizeof(varStoreInfo));
   node->varName = varName;
   node->value = val;
   strcpy(node->location,location);
   node->isConst = isConst;
   node->next = NULL;
   node->prev = varLast;
   if (varHead == NULL) {
       varHead = node;
       varLast = node;;
       varList = node;
   } else {
       //node->prev = varLast;
       varLast->next = node;
       varLast = varLast->next;
   }
   varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO FREE THE VARIABLE INFORMATION LIST
************************************************************************
*/
void FreeVarList()
{  
   varStoreInfo* tmp;
   while (varHead != NULL)
    {  
       tmp = varHead;
       varHead = varHead->next;
       free(tmp);
    }
   varLast = NULL;
}

/*
***********************************************************************
  FUNCTION TO LOOKUP VARIABLE INFORMATION FROM THE VARINFO LIST
************************************************************************
*/
char* LookUpVarInfo(char* name, long val) {
    varList = varLast;
    if (varList == NULL) printf("NULL varlist");
    while (varList != NULL) {
        if (varList->isConst == true) {
            if (varList->value == val) return varList->location;
        }
        else {
            if (!strcmp(name,varList->varName)) return varList->location;
        }
        varList = varList->prev;
    }
    varList = varHead;
    return "";
}

/*
***********************************************************************
  FUNCTION TO UPDATE VARIABLE INFORMATION 
************************************************************************
*/
void UpdateVarInfo(char* varName, char* location, long val, bool isConst) {
  
   if (!(strcmp(LookUpVarInfo(varName, val), ""))) {
       AddVarInfo(varName, location, val, isConst);
   }
   else {
       varList = varHead;
       if (varList == NULL) printf("NULL varlist");
       while (varList != NULL) {
           if (!strcmp(varList->varName,varName)) {
               varList->value = val;
               strcpy(varList->location,location);
               varList->isConst = isConst;
               break;
        }
        varList = varList->next;
       }
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE VARIABLE INFORMATION LIST
************************************************************************
*/
void PrintVarListInfo() {
    varList = varHead;
    if (varList == NULL) printf("NULL varlist");
    while (varList != NULL) {
        if (!varList->isConst) {
            printf("\t %s : %s", varList->varName, varList->location);
        }
        else {
            printf("\t %ld : %s", varList->value, varList->location);
        }
        varList = varList->next;
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO ADD NEW REGISTER INFORMATION TO THE REGISTER INFO LIST
************************************************************************
*/
void AddRegInfo(char* name, int avail) {

   regInfo* node = malloc(sizeof(regInfo));
   node->regName = name;
   node->avail = avail;
   node->next = NULL; 

   if (regHead == NULL) {
       regHead = node;
       regList = node;
       regLast = node;
   } else {
       regLast->next = node;
       regLast = node;
   }
   regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO FREE REGISTER INFORMATION LIST
************************************************************************
*/
void FreeRegList()
{  
   regInfo* tmp;
   while (regHead != NULL)
    {  
       tmp = regHead;
       regHead = regHead->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO UPDATE THE AVAILIBILITY OF REGISTERS IN THE REG INFO LIST
************************************************************************
*/
void UpdateRegInfo(char* regName, int avail) {
    while (regList != NULL) {
        if (regName == regList->regName) {
            regList->avail = avail;
        }
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO RETURN THE NEXT AVAILABLE REGISTER
************************************************************************
*/
char* GetNextAvailReg(bool noAcc) {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        if (regList->avail == 1) {
            if (!noAcc) return regList->regName;
            // if not rax and dont return accumulator set to true, return the other reg
            // if rax and noAcc == true, skip to next avail
            if (noAcc && strcmp(regList->regName, "%rax")) { 
                return regList->regName;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return "NoReg";
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF ANY REGISTER APART FROM OR INCLUDING 
  THE ACCUMULATOR(RAX) IS AVAILABLE
************************************************************************
*/
int IfAvailReg(bool noAcc) {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        if (regList->avail == 1) {
            // registers available
            if (!noAcc) return 1;
            if (noAcc && strcmp(regList->regName, "%rax")) {
                return 1;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return 0;
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF A SPECIFIC REGISTER IS AVAILABLE
************************************************************************
*/
bool IsAvailReg(char* name) {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        if (!strcmp(regList->regName, name)) {
           if (regList->avail == 1) {
               return true;
           } 
        }
        regList = regList->next;
    }
    regList = regHead;
    return false;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE REGISTER INFORMATION
************************************************************************
*/
void PrintRegListInfo() {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        printf("\t %s : %d", regList->regName, regList->avail);
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO CREATE THE REGISTER LIST
************************************************************************
*/
void CreateRegList() {
    // Create the initial reglist which can be used to store variables.
    // 4 general purpose registers : AX, BX, CX, DX
    // 4 special purpose : SP, BP, SI , DI. 
    // Other registers: r8, r9
    // You need to decide which registers you will add in the register list 
    // use. Can you use all of the above registers?

    // Since I am pushing on the stack, I never need to use the RegList.
}



/*
***********************************************************************
  THIS FUNCTION IS MEANT TO PUT THE FUNCTION ARGUMENTS ON STACK
************************************************************************
*/
int PushArgOnStack(NodeList* arguments) {
    argCounter = 0;
    lastUsedOffset = 0;
    while (arguments != NULL) {
        argCounter++;
        LongToCharOffset();
        if (argCounter == 1) {
            fprintf(fptr, "\nmovq %%rdi, %s", lastOffsetUsed);
        }
        else if (argCounter == 2) {
            fprintf(fptr, "\nmovq %%rsi, %s", lastOffsetUsed);
        }
        else if (argCounter == 3) {
            fprintf(fptr, "\nmovq %%rdx, %s", lastOffsetUsed);
        }
        else if (argCounter == 4) {
            fprintf(fptr, "\nmovq %%rcx, %s", lastOffsetUsed);
        }
        else if (argCounter == 5) {
            fprintf(fptr, "\nmovq %%r8, %s", lastOffsetUsed);
        }
        else if (argCounter == 6) {
            fprintf(fptr, "\nmovq %%r9, %s", lastOffsetUsed);
        }
        // Remember the location of the argument on the stack
        AddVarInfo(arguments->node->name, lastOffsetUsed, 0, 0);
        arguments = arguments->next;
    }
    return argCounter;
}


/*
*************************************************************************
  THIS FUNCTION IS MEANT TO GET THE FUNCTION ARGUMENTS FROM THE  STACK
**************************************************************************
*/
void PopArgFromStack(NodeList* arguments) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */
    while (arguments != NULL) {
    /*
     ***********************************************************************
              TODO : YOUR CODE HERE
      THINK ABOUT WHERE EACH ARGUMENT COMES FROM. EXAMPLE WHERE IS THE
      FIRST ARGUMENT OF A FUNCTION STORED AND WHERE SHOULD IT BE EXTRACTED
      FROM AND STORED TO..
     ************************************************************************
     */
        arguments = arguments->next;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO PROCESS EACH CODE STATEMENT AND GENERATE 
  ASSEMBLY FOR IT. 
  TIP: YOU CAN MODULARIZE BETTER AND ADD NEW SMALLER FUNCTIONS IF YOU 
  WANT THAT CAN BE CALLED FROM HERE.
 ************************************************************************
 */  
void ProcessStatements(NodeList* statements) {
    // Allocate enough space on the stack for all future calculations
    int necessary_allocations = argCounter;
    NodeList* tmp = statements;
    while (statements != NULL) {
        if (statements->node->stmtCode == ASSIGN) {
            necessary_allocations++;
        }
        statements = statements->next;
    }
    fprintf(fptr, "\nsubq $%d, %%rsp", necessary_allocations * 8);
    
    statements = tmp;
    while (statements != NULL) {
        // Perform assembly instructions depending on the assignment
        Node* node = statements->node;
        char* varLocation;
        if (node->stmtCode == ASSIGN) {
            Node* stmtNodeRight = node->right;
            LongToCharOffset();

            // Case 1: Constant
            if (stmtNodeRight->exprCode == CONSTANT) {
                AddVarInfo(node->name, lastOffsetUsed, stmtNodeRight->value, true);
                fprintf(fptr, "\nmovq $%d, %s", stmtNodeRight->value, lastOffsetUsed);
            }
            // Case 2: Single variable
            else if (stmtNodeRight->exprCode == VARIABLE) {
                varLocation = LookUpVarInfo(stmtNodeRight->name, 0);
                AddVarInfo(node->name, lastOffsetUsed, 0, 0);
                fprintf(fptr, "\nmovq %s, %s", varLocation, lastOffsetUsed);
            }
            // Case 3: Unary operation
            else if (stmtNodeRight->opCode == NEGATE) {
                varLocation = LookUpVarInfo(stmtNodeRight->left->name, 0);
                AddVarInfo(node->name, lastOffsetUsed, 0, 0);
                fprintf(fptr, "\nmovq %s, %%rax", varLocation);
                fprintf(fptr, "\nnegq %%rax");
                fprintf(fptr, "\nmovq %%rax, %s", lastOffsetUsed);
            }
            // Case 4: Function call
            else if (stmtNodeRight->opCode == FUNCTIONCALL) {
                NodeList* arg = stmtNodeRight->arguments;
                int curr_param = 0;
                while (arg != NULL) {
                    curr_param++;
                    if (curr_param == 1) {
                        if (arg->node->exprCode == VARIABLE) {
                            varLocation = LookUpVarInfo(arg->node->name, 0);
                            fprintf(fptr, "\nmovq %s, %%rdi", varLocation);
                        }
                        else {
                            fprintf(fptr, "\nmovq $%d, %%rdi", arg->node->value);
                        }
                    }
                    else if (curr_param = 2) {
                        if (arg->node->exprCode == VARIABLE) {
                            varLocation = LookUpVarInfo(arg->node->name, 0);
                            fprintf(fptr, "\nmovq %s, %%rsi", varLocation);
                        }
                        else {
                            fprintf(fptr, "\nmovq $%d, %%rsi", arg->node->value);
                        }
                    }
                    else if (curr_param = 3) {
                        if (arg->node->exprCode == VARIABLE) {
                            varLocation = LookUpVarInfo(arg->node->name, 0);
                            fprintf(fptr, "\nmovq %s, %%rdx", varLocation);
                        }
                        else {
                            fprintf(fptr, "\nmovq $%d, %%rdx", arg->node->value);
                        }
                    }
                    else if (curr_param = 4) {
                        if (arg->node->exprCode == VARIABLE) {
                            varLocation = LookUpVarInfo(arg->node->name, 0);
                            fprintf(fptr, "\nmovq %s, %%rcx", varLocation);
                        }
                        else {
                            fprintf(fptr, "\nmovq $%d, %%rcx", arg->node->value);
                        }
                    }
                    else if (curr_param = 5) {
                        if (arg->node->exprCode == VARIABLE) {
                            varLocation = LookUpVarInfo(arg->node->name, 0);
                            fprintf(fptr, "\nmovq %s, %%r8", varLocation);
                        }
                        else {
                            fprintf(fptr, "\nmovq $%d, %%r8", arg->node->value);
                        }
                    }
                    else if (curr_param = 6) {
                        if (arg->node->exprCode == VARIABLE) {
                            varLocation = LookUpVarInfo(arg->node->name, 0);
                            fprintf(fptr, "\nmovq %s, %%r9", varLocation);
                        }
                        else {
                            fprintf(fptr, "\nmovq $%d, %%r9", arg->node->value);
                        }
                    }
                    arg = arg->next;
                }
                AddVarInfo(node->name, lastOffsetUsed, 0, 0);
                fprintf(fptr, "\ncallq %s", stmtNodeRight->left->name);
                fprintf(fptr, "\nmovq %%rax, %s", lastOffsetUsed);
            }
            // Case 5: Binary operation
            else {
                if (stmtNodeRight->left->exprCode == VARIABLE) {
                    varLocation = LookUpVarInfo(stmtNodeRight->left->name, 0);
                    fprintf(fptr, "\nmovq %s, %%rax", varLocation);
                }
                else {
                    fprintf(fptr, "\nmovq $%d, %%rax", stmtNodeRight->left->value);
                }

                if (stmtNodeRight->right->exprCode == VARIABLE) {
                    varLocation = LookUpVarInfo(stmtNodeRight->right->name, 0);
                    fprintf(fptr, "\nmovq %s, %%rcx", varLocation);
                }
                else {
                    fprintf(fptr, "\nmovq $%d, %%rcx", stmtNodeRight->right->value);
                }
                switch(stmtNodeRight->opCode) {
                    case MULTIPLY:
                        fprintf(fptr, "\nimulq %%rcx, %%rax");
                        break;
                    case DIVIDE:
                        fprintf(fptr, "\ncqto");
                        fprintf(fptr, "\nidivq %%rcx");
                        break;
                    case ADD:
                        fprintf(fptr, "\naddq %%rcx, %%rax");
                        break;
                    case SUBTRACT:
                        fprintf(fptr, "\nsubq %%rcx, %%rax");
                        break;
                    case BOR:
                        fprintf(fptr, "\norq %%rcx, %%rax");
                        break;
                    case BAND:
                        fprintf(fptr, "\nandq %%rcx, %%rax");
                        break;
                    case BXOR:
                        fprintf(fptr, "\nxorq %%rcx, %%rax");
                        break;
                    case BSHR:
                        fprintf(fptr, "\nsarq %%rcx, %%rax");
                        break;
                    case BSHL:
                        fprintf(fptr, "\nsalq %%rcx, %%rax");
                        break;
                    default:
                        break;
                }
                AddVarInfo(node->name, lastOffsetUsed, 0, 0);
                fprintf(fptr, "\nmovq %%rax, %s", lastOffsetUsed);
            }
        }
        // Return statement
        else {
            if (node->left->exprCode == VARIABLE) {
                varLocation = LookUpVarInfo(node->left->name, 0);
                fprintf(fptr, "\nmovq %s, %%rax", varLocation);
            }
            else {
                fprintf(fptr, "\nmovq $%d, %%rax", node->left->value);
            }
            fprintf(fptr, "\naddq $%d, %%rsp", necessary_allocations * 8);
            RetAsm();
        }
        statements = statements->next;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO DO CODEGEN FOR ALL THE FUNCTIONS IN THE FILE
 ************************************************************************
*/
void Codegen(NodeList* worklist) {
    fptr = fopen("assembly.s", "w+");
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */
    if (fptr == NULL) {
        printf("\n Could not create assembly file");
        return; 
    }
    while (worklist != NULL) {
        Node* function = worklist->node;
        // Initialize function
        InitAsm(function->name);
        // Push all arguments onto stack
        PushArgOnStack(function->arguments);
        ProcessStatements(function->statements);

        worklist = worklist->next; 
    }
    fclose(fptr);
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS BELOW THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/


