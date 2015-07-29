/*
  This file is part of libanio (https://github.com/yoones/libanio).

  libanio is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  libanio is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with libanio.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef LIBANIO_INTERNAL_INTERFACE_H_
# define LIBANIO_INTERNAL_INTERFACE_H_

/**
 * @file
 */

# include "libanio_types.h"

/**
 * @brief Starts the monitor on a separate thread (that'll create workers and wait for events)
 *
 * Exemples:
 *
 * libanio_start_monitor(&server);
 *
 * @param server Pointer to the t_anio structure which monitor is to be started.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_start_monitor(t_anio *server);

/**
 * @brief Stops the monitor and its workers
 *
 * Exemples:
 *
 * libanio_stop_monitor(&server);
 *
 * @param server Pointer to the t_anio structure which monitor is to be stoped.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_stop_monitor(t_anio *server);


/**
 * @brief Creates the threads (workers) that'll handle events polled by their monitor.
 * This function is called by the monitor, end-user should not call it.
 *
 * Exemples:
 *
 * libanio_create_workers(&server);
 *
 * @param server Pointer to the t_anio structure which will hold the workers (and their monitor).
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_create_workers(t_anio *server);

/**
 * @brief Asks the workers to exit.
 * This function is called by the monitor, end-user should not call it.
 *
 * Exemples:
 *
 * libanio_destroy_workers(&server);
 *
 * @param server Pointer to the t_anio structure which will hold the workers (and their monitor).
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_destroy_workers(t_anio *server);

/**
 * @brief Handle (read|write|accept|eof|error) event.
 * This function is called by the monitor, end-user should not call it.
 *
 * Exemples:
 *
 * libanio_handle_event(&server, &my_job);
 *
 * @param server Pointer to the t_anio structure which will hold the workers (and their monitor).
 * @param job Pointer to the epoll_event containing the event and the associated file descriptor.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_handle_event(t_anio *server, struct epoll_event *job);

/**
 * @brief Initializes a t_fdesc structure with the five file descriptor.
 *
 * Exemples:
 *
 * libanio_fdesc_init(&server, client_fd);
 *
 * @param fdesc Pointer to the t_fdesc structure to initialize.
 * @param fd File descriptor.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_fdesc_init(t_fdesc *fdesc, int fd);

/**
 * @brief Closes the file descriptor and frees the t_fdesc structure's internal buffers.
 *
 * Exemples:
 *
 * libanio_fdesc_close(&server);
 *
 * @param fdesc Pointer to the t_fdesc structure to clean.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_fdesc_close(t_fdesc *fdesc);

/**
 * @brief Checks if a file descriptor exists in the clients list.
 *
 * Exemples:
 *
 * libanio_has_client(&server, client_fd);
 *
 * @param server Pointer to the t_anio structure holding the clients to look into.
 * @param fd File descriptor to look for.
 *
 * @return (1) if client is found
 * @return (0) if client is not found
 * @return (-1) on error
 */
int		libanio_has_client(t_anio *server, int fd);

/**
 * @brief Adds a new client.
 *
 * Exemples:
 *
 * libanio_add_client(&server, client_fd);
 *
 * @param server Pointer to the t_anio structure holding the clients list.
 * @param fd File descriptor to add as a client.
 *
 * @return (1) if client is found
 * @return (0) if client is not found
 * @return (-1) on error
 */
int		libanio_add_client(t_anio *server, int fd);

/**
 * @brief Removes a client (and closes it).
 *
 * Exemples:
 *
 * libanio_remove_client(&server, client_fd);
 *
 * @param server Pointer to the t_anio structure holding the clients list.
 * @param fd File descriptor to remove from the clients list.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_remove_client(t_anio *server, int fd);

/**
 * @brief Gets the file descriptor's associated t_fdesc structure if found.
 *
 * Exemples:
 *
 * libanio_get_client(&server, client_fd, &fdesc);
 *
 * @param server Pointer to the t_anio structure holding the clients to look into.
 * @param fd File descriptor to look for.
 * @param fdesc Will contain the t_fdesc structure address if found.
 *
 * @return (1) if client is found
 * @return (0) if client is not found
 * @return (-1) on error
 */
int		libanio_get_client(t_anio *server, int fd, t_fdesc **fdesc);

/*
 * The following functions just do error handling if needed (display error message on errors, loop on EINTR)
 */

/* pthread helpers */
int		x_pthread_mutex_lock(pthread_mutex_t *mutex);
int		x_pthread_mutex_trylock(pthread_mutex_t *mutex);
int		x_pthread_mutex_unlock(pthread_mutex_t *mutex);
int		x_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
				 void *(*start_routine) (void *), void *arg);
int		x_pthread_cancel(pthread_t thread);
int		x_pthread_cond_broadcast(pthread_cond_t *cond);
int		x_pthread_cond_wait(pthread_cond_t *cond,
				    pthread_mutex_t *mutex);

/* epoll helpers */

int		x_epoll_create1(int flags);
int		x_epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
int		x_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

#endif /* LIBANIO_INTERNAL_INTERFACE_H_ */
