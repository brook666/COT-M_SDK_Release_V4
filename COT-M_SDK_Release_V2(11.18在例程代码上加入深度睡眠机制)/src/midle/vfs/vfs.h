/*!
 * \file vfs.h
 * \brief virtual file system
 */

#ifndef _VFS_H_H
#define _VFS_H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef void (*irq_handler_callback_t)(void);


struct File;
/*!
 * \brief file operation
 */
struct FileOps
{
    int32_t (*open)( struct File* file, uint32_t args );   /*!< required */
    int32_t (*release)( struct File* file );/*!< required */
    int32_t (*read)( struct File* file, void* buf, uint32_t count, uint32_t* pos );  /*!< no required*/
    int32_t (*write)( struct File* file, void* buf, uint32_t count, uint32_t* pos ); /*!< no required*/
    int32_t (*lseek)( struct File* file, int32_t offset, uint8_t whence );            /*!< no required*/
    int32_t (*ioctl)( struct File* file, int32_t request, uint32_t args );              /*!< no required*/
    int32_t (*irq_handler_register)( struct File* file, int32_t request, uint32_t args,irq_handler_callback_t callback ); /*!< no required*/
    //int32_t (*irq_handler_register)( struct File* file, int32_t request, uint32_t args,void (*callback) ); /*!< no required*/
};


/*!
 * \brief file node
 */
struct File
{
    const char* path;  			/*!< file path*/
    uint32_t pos;   			/*!< operation pos*/
    struct FileOps* ops;		/*!< file operation */
    uint32_t* private_pramaer;	/*!< private data for each file */
    uint8_t count;    			/*!< file reference count*/
};


/*!
 * \brief open file
 * \param [in] path - file path or file name
 * \param [in] args - arguments. Set zero when unused.
 * \return file descriptor or -1 if an error occurred.
 */
int32_t open( const char* path, uint32_t args );


/*!
 * \brief close file
 * \param [in] fd - file descriptor
 * \return 0 on success, -1 on error.
 */
int32_t close( int32_t fd );

/*!
 * \brief read file
 * \param [in] fd - file descriptor
 * \param [out] buf -data read from file
 * \param [in] count -data length to be read
 * \return the number of bytes read
 */
int32_t read( int32_t fd, void* buf, uint32_t count );


/*!
 * \brief write data to file
 * \param [in] fd - file descriptor
 * \param [in] buf - data writen to file
 * \param [in] count - data length to be writen
 * \return the number of bytes writen
 */
int32_t write( int32_t fd, void* buf, uint32_t count );

/*!
 * \brief reposition file offset
 * \param [in] fd - file descriptor
 * \param [in] offset - offset from whence
 * \param [in] whence - unused now. set it 0.
 * \return new file offset
 */
int32_t lseek( int32_t fd, uint32_t offset, uint8_t whence );

/*!
 * \brief control device
 * \param [in] fd - device descriptor
 * \param [in] request - request code
 * \param [in] args - arguments
 * \return zero on success, -1 on error
 */
int32_t ioctl( int32_t fd, int32_t request, uint32_t args );


/*!
 * \brief  device irq handler callback register
 * \param [in] fd - device descriptor
 * \param [in] request - request code
 * \param [in] args - arguments
 * \param [in] callback - dev irq callback function
 * \return zero on success, -1 on error
 */
int32_t irq_handler_register( int32_t fd, int32_t request, uint32_t args,irq_handler_callback_t callback);


/*!
 * \brief init vfs
 * \return void
 */
void init_vfs( void );

/*!
 * \brief 注册设备文件
 * \param [in] file - device descriptor
 * \return zero on success, -1 on error
 */
int32_t register_dev( struct File* file );

#ifdef __cplusplus
}
#endif

#endif

