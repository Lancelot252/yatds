        #include "queue.h"
        #include <stdio.h>
        #include <stdlib.h>

        static void
        Die( const char *Message )
        {
            fprintf( stderr, "%s\n", Message );
            exit( EXIT_FAILURE );
        }

        #define MinQueueSize ( 5 )

        struct QueueRecord
        {
            int Capacity;
            int Front;
            int Rear;
            int Size;
            ElementType *Array;
        };

/* START: fig3_58.txt */
        int
        IsEmpty( Queue Q )
        {
            return Q->Size == 0;
        }
/* END */

        int
        IsFull( Queue Q )
        {
            return Q->Size == Q->Capacity;
        }

        Queue
        CreateQueue( int MaxElements )
        {
            Queue Q;

/* 1*/      if( MaxElements < MinQueueSize )
/* 2*/          Die( "Queue size is too small" );

/* 3*/      Q = malloc( sizeof( struct QueueRecord ) );
/* 4*/      if( Q == NULL )
/* 5*/          Die( "Out of space!!!" );

/* 6*/      Q->Array = malloc( sizeof( ElementType ) * MaxElements );
/* 7*/      if( Q->Array == NULL )
/* 8*/          Die( "Out of space!!!" );
/* 9*/      Q->Capacity = MaxElements;
/*10*/      MakeEmpty( Q );

/*11*/      return Q;
        }

/* START: fig3_59.txt */
        void
        MakeEmpty( Queue Q )
        {
            Q->Size = 0;
            Q->Front = 1;
            Q->Rear = 0;
        }
/* END */

        void
        DisposeQueue( Queue Q )
        {
            if( Q != NULL )
            {
                free( Q->Array );
                free( Q );
            }
        }

/* START: fig3_60.txt */

        static int
        Succ( int Value, Queue Q )
        {
            if( ++Value == Q->Capacity )
                Value = 0;
            return Value;
        }

        void
        Enqueue( ElementType X, Queue Q )
        {
            if( IsFull( Q ) )
                Die( "Full queue" );
            else
            {
                Q->Size++;
                Q->Rear = Succ( Q->Rear, Q );
                Q->Array[ Q->Rear ] = X;
            }
        }
/* END */



        ElementType
        Front( Queue Q )
        {
            if( !IsEmpty( Q ) )
                return Q->Array[ Q->Front ];
            Die( "Empty queue" );
            return 0;  /* Return value used to avoid warning */
        }

        void
        Dequeue( Queue Q )
        {
            if( IsEmpty( Q ) )
                Die( "Empty queue" );
            else
            {
                Q->Size--;
                Q->Front = Succ( Q->Front, Q );
            }
        }

        ElementType
        FrontAndDequeue( Queue Q )
        {
            ElementType X = 0;

            if( IsEmpty( Q ) )
                Die( "Empty queue" );
            else
            {
                Q->Size--;
                X = Q->Array[ Q->Front ];
                Q->Front = Succ( Q->Front, Q );
            }
            return X;
        }
