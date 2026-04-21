        #include "stackli.h"
        #include <stdio.h>
        #include <stdlib.h>

        static void
        Die( const char *Message )
        {
            fprintf( stderr, "%s\n", Message );
            exit( EXIT_FAILURE );
        }

        struct Node
        {
            ElementType Element;
            PtrToNode   Next;
        };

/* START: fig3_40.txt */
        int
        IsEmpty( Stack S )
        {
            return S->Next == NULL;
        }
/* END */

/* START: fig3_41.txt */
        Stack
        CreateStack( void )
        {
            Stack S;

            S = malloc( sizeof( struct Node ) );
            if( S == NULL )
                Die( "Out of space!!!" );
            S->Next = NULL;
            MakeEmpty( S );
            return S;
        }

        void
        MakeEmpty( Stack S )
        {
            if( S == NULL )
                Die( "Must use CreateStack first" );
            else
                while( !IsEmpty( S ) )
                    Pop( S );
        }
/* END */

        void
        DisposeStack( Stack S )
        {
            MakeEmpty( S );
            free( S );
        }

/* START: fig3_42.txt */
        void
        Push( ElementType X, Stack S )
        {
            PtrToNode TmpCell;

            TmpCell = malloc( sizeof( struct Node ) );
            if( TmpCell == NULL )
                Die( "Out of space!!!" );
            else
            {
                TmpCell->Element = X;
                TmpCell->Next = S->Next;
                S->Next = TmpCell;
            }
        }
/* END */

/* START: fig3_43.txt */
        ElementType
        Top( Stack S )
        {
            if( !IsEmpty( S ) )
                return S->Next->Element;
            Die( "Empty stack" );
            return 0;  /* Return value used to avoid warning */
        }
/* END */

/* START: fig3_44.txt */
        void
        Pop( Stack S )
        {
            PtrToNode FirstCell;

            if( IsEmpty( S ) )
                Die( "Empty stack" );
            else
            {
                FirstCell = S->Next;
                S->Next = S->Next->Next;
                free( FirstCell );
            }
        }
/* END */
