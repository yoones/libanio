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

#ifndef LIBANIO_INTERFACE_H_
# define LIBANIO_INTERFACE_H_

/**
 * @file
 */

# include "libanio_types.h"

# define ANIO_BUF_SIZE 4096

/**
 * @brief Initializes the server structure
 *
 * Exemples:
 *
 * libanio_init(&server, server_fd, 0, 21, &on_accept, &on_read, &on_eof, &on_error, ANIO_MODE_STREAM);
 *
 * libanio_init(&server, server_fd, 14, 20, &on_accept, &on_read, &on_eof, NULL, ANIO_MODE_BLOCK, 1024);
 *
 * libanio_init(&server, server_fd, 100, 100, &on_accept, &on_read, &on_eof, &on_error, ANIO_MODE_LINE, "\r\n");
 *
 * @param server Pointer to the t_anio structure to initialize.
 * @param fd File description on already listening socket.
 * @param max_clients Maximum simultaneously-connected clients allowed.
 * @param thread_pool_size Maximum number of threads to perform i/o operations and callbacks when needed.
 * @param fptr_on_accept Function pointer to be called when accepting a new client.
 * @param fptr_on_read Function pointer to be called when data is read. The data's readiness to be consumed depends on the mode set on initialization: stream, block or line. Must not be NULL.
 * @param fptr_on_eof Function pointer to be called when reaching end-of-file.
 * @param fptr_on_error Function pointer to be called when a syscall fails.
 * @param server mode Defines when to call the fptr_on_read callback (ANIO_STREAM: whenever data is read, ANIO_BLOCK: when a complete block is available, ANIO_LINE: when a complete line is available). Once set, the mode shall not be changed.
 * @param ... Depending on the mode: ANIO_STREAM: nothing (ignored if any given), ANIO_BLOCK: block size of type (size_t), ANIO_LINE: array of chars representing the newline sequence of type (char *).
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_init(t_anio *server,
			     int fd,
			     size_t max_clients,
			     size_t thread_pool_size,
			     t_anio_fptr_on_accept fptr_on_accept,
			     t_anio_fptr_on_read fptr_on_read,
			     t_anio_fptr_on_eof fptr_on_eof,
			     t_anio_fptr_on_error fptr_on_error,
			     t_anio_mode mode, ...);

/**
 * @brief Cleans the t_anio structure
 *
 * Exemple:
 *
 * libanio_free(&server);
 *
 * @param server Pointer to the t_anio structure to clean.
 */
void		libanio_free(t_anio *server);

/**
 * @brief Sets the maximum allowed number of simultaneously-connected clients.
 *
 * Exemples:
 *
 * libanio_set_max_clients(&server, 1024);
 *
 * @param server Pointer to the t_anio server structure.
 * @param nb Number of simultaneously-connected clients allowed. If set to 0, no limit is applied.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_set_max_clients(t_anio *server,
					size_t nb);

/**
 * @brief Sets the number of threads to use to perform i/o operations and callbacks when needed.
 *
 * Exemples:
 *
 * libanio_set_thread_pool_size(&server, 20);
 *
 * @param server Pointer to the t_anio server structure.
 * @param size Maximum number of threads to perform i/o operations and callbacks when needed. Size must be a strictly positive number
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_set_thread_pool_size(t_anio *server,
					     size_t size);


/**
 * @brief Sets the callbacks function pointers
 *
 * Exemples:
 *
 * libanio_set_callbacks(&server, &on_accept, &on_read, &on_eof, &on_error);
 *
 * libanio_set_callbacks(&server, NULL, &on_read, NULL, NULL);
 *
 * libanio_set_callbacks(&server, NULL, NULL, NULL, NULL);
 *
 * @param server Pointer to the t_anio structure.
 * @param fptr_on_accept Function pointer to be called when accepting a new client.
 * @param fptr_on_read Function pointer to be called when data is read. The data's readiness to be consumed depends on the mode set on initialization: stream, block or line. Must not be NULL.
 * @param fptr_on_eof Function pointer to be called when reaching end-of-file.
 * @param fptr_on_error Function pointer to be called when a syscall fails.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_set_callbacks(t_anio *server,
				      t_anio_fptr_on_accept fptr_on_accept,
				      t_anio_fptr_on_read fptr_on_read,
				      t_anio_fptr_on_eof fptr_on_eof,
				      t_anio_fptr_on_error fptr_on_error);

/**
 * @brief Sets the function pointer to be called when accepting a new client.
 *
 * Exemples:
 *
 * libanio_set_callback_on_accept(&server, &on_accept);
 *
 * @param server Pointer to the t_anio structure.
 * @param fptr_on_accept Function pointer to be called when accepting a new client.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_set_callback_on_accept(t_anio *server,
					       t_anio_fptr_on_accept fptr_on_accept);

/**
 * @brief Sets the function pointer to be called when data is read and ready to be consumed.
 *
 * Exemples:
 *
 * libanio_set_callback_on_read(&server, &on_accept);
 *
 * @param server Pointer to the t_anio structure.
 * @param fptr_on_read Function pointer to be called when data is read. The data's readiness to be consumed depends on the mode set on initialization: stream, block or line. Must not be NULL.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_set_callback_on_read(t_anio *server,
					     t_anio_fptr_on_read fptr_on_read);

/**
 * @brief Sets the function pointer to be called when reaching end-of-file.
 *
 * Exemples:
 *
 * libanio_set_callback_on_eof(&server, &on_eof);
 *
 * @param server Pointer to the t_anio structure.
 * @param fptr_on_eof Function pointer to be called when reaching end-of-file.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_set_callback_on_eof(t_anio *server,
					    t_anio_fptr_on_eof fptr_on_eof);

/**
 * @brief Sets the function pointer to be called when a syscall fails.
 *
 * Exemples:
 *
 * libanio_set_callback_on_error(&server, &on_error);
 *
 * @param server Pointer to the t_anio structure.
 * @param fptr_on_error Function pointer to be called when a syscall fails.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_set_callback_on_error(t_anio *server,
					      t_anio_fptr_on_error fptr_on_error);

/**
 * @brief Tells if the server is still running.
 *
 * Exemples:
 *
 * libanio_is_server_alive(&server);
 *
 * @param server Pointer to the t_anio structure.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_is_server_alive(t_anio *server);

/**
 * @brief Stores data to be sent (later) to a client.
 *
 * Exemples:
 *
 * libanio_send(&server, client_fd, "hello world", 11);
 *
 * @param server Pointer to the t_anio structure.
 * @param fd Client's file descriptor.
 * @param buf Pointer to the data to send.
 * @param nbytes Number of bytes to send.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_send(t_anio *server, int fd, char *buf, size_t nbytes);

/**
 * @brief Stores a string (NULL terminated array of chars) to be sent (later) to a client.
 *
 * Exemples:
 *
 * libanio_send(&server, client_fd, "hello world");
 *
 * @param server Pointer to the t_anio structure.
 * @param fd Client's file descriptor.
 * @param buf Pointer to the string.
 *
 * @return (0) on success
 * @return (-1) on error
 */
int		libanio_send_string(t_anio *server, int fd, char *buf);

#endif /* LIBANIO_INTERFACE_H_ */
