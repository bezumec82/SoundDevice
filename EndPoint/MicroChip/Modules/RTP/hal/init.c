/*! @file init.c
 * Contains initialization routines.
 */

#include "driver.h"

/*! @addtogroup Exported Exported to kernel functions
 * @{ --------------------------------------------------------------------------------------------*/

/*!
 * Main initialization function.
 * Called from 'mod_init'.
 * @param context       : #context struct. Not implemented.
 * @param initstruct    : Initialization structure. Contains c++ - like constructor and destructor.
 * @param nitems        : Amount of items to initialize.
 * @param enst          : Array of the strings
 * @param nenstr        : Amount of the string in the 'enstr' array.
 */
int init_startup    (
                    void * context,
                    struct init_struct * initstruct,
                    int nitems,
                    char ** enstr,
                    int nenstr
                    ) {
    int i, error;
    /// -1 Set loop through 'nitems'
    for (i = 0; i < nitems; i++) {
        int do_init;
        /// -2 Check if facility should be initialized -> Check #init_struct.enabled flag.
        do_init = initstruct[i].enabled;       

/// @bug
/// -# NOT USED
/// -# If init string is not NULL and enable name has the same value,
/// than initilization already passed.
/// @code        
        if ( 
        initstruct[i].enable_name && enstr
        ) {
            int j;
            for (j = 0; j < nenstr; j++) {
                if  (   enstr[j] /* Init string is not NULL */
                        /* Init string and enable name are the same */
                        && !strcmp(enstr[j], initstruct[i].enable_name)
                    ) {
                    do_init = 0;
                    // do_init = 1;
                    __PRINTK(RED, "%s already initialized.\n", initstruct[i].enable_name);
                    break;
                } //end if  (   enstr[j]
            } //end for (j = 0
        } //end if (initstruct[i].enable_name && enstr)
        /* End check if already initialized */
/// @endcode
        
        /// -2 If not - call fot #init_struct.constructor.
        if (do_init) {
            error = initstruct[i].constructor(context);
            if (error < 0) {
                init_shutdown   (
                                context, 
                                initstruct, 
                                nitems
                                );
                return error;
            } //end if (error < 0)
            /// -3 if initialization passed successfully , set #init_struct.initialized flag.
            initstruct[i].initialized = 1;
        } //end if (do_init)
    } //end for (i = 0; i < nitems; i++)
    return 0;
}
/*- End of function ------------------------------------------------------------------------------*/

/*! Cleanup function.
 * @param context       : #context struct. Not implemented.
 * @param initstruct    : Initialization structure. Contains c++ - like constructor and destructor.
 * @param nitems        : Amount of items to initialize.
 */
void init_shutdown  (
                    void *context, 
                    struct init_struct * initstruct, 
                    int nitems
                    ) {
    int i;
    for (i = nitems - 1; i >= 0; i--) {
        if (initstruct[i].initialized) {
            initstruct[i].destructor(context);
            initstruct[i].initialized = 0;
        } //end if
    } //end for
}
/*! @} -------------------------------------------------------------------------------------------*/

EXPORT_SYMBOL(init_startup);
EXPORT_SYMBOL(init_shutdown);
/*--- End of the file ----------------------------------------------------------------------------*/
