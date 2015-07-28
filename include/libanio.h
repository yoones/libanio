#ifndef LIBANIO_H_
# define LIBANIO_H_

/**
 * @file
 */

# include <stddef.h>
# include <pthread.h>
# include <sys/epoll.h>
# include <string.h>
# include <errno.h>

# include "list.h"
# include "libanio_types.h"
# include "libanio_interface.h"
# include "libanio_internal_interface.h"

/* tmp for debug */
# define RESET   "\033[0m"
# define BLACK   "\033[30m"      /* Black */
# define RED     "\033[31m"      /* Red */
# define GREEN   "\033[32m"      /* Green */
# define YELLOW  "\033[33m"      /* Yellow */
# define BLUE    "\033[34m"      /* Blue */
# define MAGENTA "\033[35m"      /* Magenta */
# define CYAN    "\033[36m"      /* Cyan */
# define WHITE   "\033[37m"      /* White */
# define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
# define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
# define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
# define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
# define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
# define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
# define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
# define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

# define print_err(errnumber) dprintf(2, RED "%s:%s:%d: %s" RESET "\n", \
				      __FILE__, __func__, __LINE__, strerror(errnumber))

# define print_custom_err(msg) dprintf(2, RED "%lu\t%s:%s:%d: %s" RESET "\n", \
				       pthread_self(), __FILE__, __func__, __LINE__, msg)

/* # define DEBUG_MODE */

# ifdef DEBUG_MODE
#  define DEBUG(color, m, args...) dprintf(2, color m RESET "\n" , ##args)
#  define DEBUG_IN() print_custom_err("IN")
#  define DEBUG_OUT() print_custom_err("OUT")
# else
#  define DEBUG(color, m, args...) ;
#  define DEBUG_IN() ;
#  define DEBUG_OUT() ;
# endif

/* --- */

#endif
