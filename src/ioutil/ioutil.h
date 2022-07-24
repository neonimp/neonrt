/**
 * @brief IO utilities
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#ifndef NEON_SRC_IOUTIL_IOUTIL_H
#define NEON_SRC_IOUTIL_IOUTIL_H
#include "../neon_common.h"
#include "../containers/hashmap.h"
#include "../containers/buffer.h"

struct rt_io_fptr {
  int32_t fdscr;
  size_t fsize;
  size_t fpos;
  neon_buff_t *fname;
  neon_buff_t *fbuff;
  bool fis_open;
  bool fis_read;
  bool fis_write;
  int32_t errno;
};

/**
 * @brief Open a file for reading.
 * @param fname Name of the file to open.
 * @param buffer_sz Size of the buffer for buffered reading.
 * @param is_read If true, the file will be opened for reading.
 * @param is_write If true, the file will be opened for writing.
 * @return A pointer to an initialized rt_io_fptr instance.
 */
extern struct rt_io_fptr *rt_io_fopen(neon_buff_t *fname, size_t buffer_sz, bool is_read, bool is_write);

/**
 * @brief Close a file.
 * @param fptr Reference to the file to close.
 */
extern void rt_io_fclose(struct rt_io_fptr *fptr);

#endif // NEON_SRC_IOUTIL_IOUTIL_H
#ifdef __cplusplus
}
#endif
