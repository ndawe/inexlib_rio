// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file inlib.license for terms.

#ifndef inlib_root_file
#define inlib_root_file

#include "ifile.h"
#include "directory.h"
#include "infos.h"
#include "free_seg.h"

#include "../platform.h"
#include "../path.h"

#include <map>
#include <string>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#ifdef _MSC_VER
    #include <direct.h>
    #include <io.h>
    // disable the warning about the usage of "this" in the constructor.
    #pragma warning(disable:4355)
#else
    #include <unistd.h>
#endif

namespace inlib {
    namespace root {

        class file : public virtual ifile {
            static const std::string& s_class()
            {
                static const std::string s_v("inlib::root::file");
                return s_v;
            }
            static int not_open()
            {
                return -1;
            }
            static uint32 kBegin()
            {
                return 64;
            }
            static uint32 START_BIG_FILE()
            {
                return 2000000000;
            }
        public: //ifile
            virtual const std::string& path() const
            {
                return m_path;
            }
            virtual bool verbose() const
            {
                return m_verbose;
            }
            virtual std::ostream& out() const
            {
                return m_out;
            }

            virtual bool byte_swap() const
            {
                return is_little_endian();
            }
            virtual bool set_pos(seek a_offset = 0, from a_from = begin)
            {
                int whence = 0;

                switch (a_from) {
                    case begin:
                        whence = SEEK_SET;
                        break;

                    case current:
                        whence = SEEK_CUR;
                        break;

                    case end:
                        whence = SEEK_END;
                        break;
                }

                #if defined(__linux__) && (__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 2)

                if (::lseek64(m_file, a_offset, whence) < 0) {
                #elif defined(_MSC_VER)

                if (::_lseeki64(m_file, a_offset, whence) < 0) {
                #else

                if (::lseek(m_file, a_offset, whence) < 0) {
                #endif
                    m_out << "inlib::root::file::set_pos :"
                          << " cannot set position " << a_offset
                          << " in file " << sout(m_path) << "."
                          << std::endl;
                    return false;
                }

                return true;
            }

            virtual seek END() const
            {
                return m_END;
            }
            virtual void set_END(seek a_end)
            {
                m_END = a_end;

                if (m_free_segs.empty()) {
                    m_out << "inlib::root::file::set_END :"
                          << " free_seg list should not be empty here."
                          << std::endl;
                } else {
                    free_seg* end_seg = m_free_segs.back();

                    if (end_seg->last() != START_BIG_FILE()) {
                        m_out << "inlib::root::file::set_END :"
                              << " last free_seg is not the ending of file one."
                              << " free_seg list looks corrupted."
                              << std::endl;
                    } else {
                        m_free_segs.back()->set_first(m_END);
                    }
                }
            }

            virtual bool read_buffer(char* a_buffer, uint32 a_length)
            {
                // Read a buffer from the file.
                // This is the basic low level read operation.
                #ifdef _MSC_VER
                typedef int ssize_t;
                #endif
                ssize_t siz;

                while ((siz = ::read(m_file, a_buffer, a_length)) < 0 &&
                       error_number() == EINTR) reset_error_number();

                if (siz < 0) {
                    m_out << "inlib::root::file::read_buffer :"
                          << " error reading from file " << sout(m_path) << "."
                          << std::endl;
                    return false;
                }

                if (siz != ssize_t(a_length)) {
                    m_out << "inlib::root::file::read_buffer :"
                          << " error reading all requested bytes from file "
                          << sout(m_path) << ", got " << long_out(siz)
                          << " of " << a_length
                          << std::endl;
                    return false;
                }

                m_bytes_read += siz;
                return true;
            }

            virtual bool write_buffer(const char* a_buffer, uint32 a_length)
            {
                // Write a buffer to the file. This is the basic low level write operation.
                #ifdef _MSC_VER
                typedef int ssize_t;
                #endif
                ssize_t siz;

                while ((siz = ::write(m_file, a_buffer, a_length)) < 0 &&
                       error_number() == EINTR) reset_error_number();

                if (siz < 0) {
                    m_out << "inlib::root::file::write_buffer :"
                          << " error writing to file " << sout(m_path) << "."
                          << std::endl;
                    return false;
                }

                if (siz != (ssize_t)a_length) {
                    m_out << "inlib::root::file::write_buffer :"
                          << "error writing all requested bytes to file " << sout(m_path)
                          << ", wrote " << long_out(siz) << " of " << a_length
                          << std::endl;
                    return false;
                }

                //m_bytes_write  += siz;
                return true;
            }

            virtual uint32 version() const
            {
                // Return version id as an integer, i.e. "2.22/04" -> 22204.
                static const uint32 ROOT_MAJOR_VERSION = 4;
                static const uint32 ROOT_MINOR_VERSION = 0;
                static const uint32 ROOT_PATCH_VERSION = 0;
                return
                    10000 * ROOT_MAJOR_VERSION +
                    100 * ROOT_MINOR_VERSION +
                    ROOT_PATCH_VERSION;
            }

            virtual bool synchronize()
            {
                // Synchornize a file's in-core and on-disk states.
                #ifdef _MSC_VER
                if (::_commit(m_file)) {
                    m_out << "inlib::root::file::synchronize :"
                          << " in _commit() for file " << sout(m_path) << "."
                          << std::endl;
                    return false;
                }

                #elif defined(__MINGW32__) || defined(__MINGW64__)
                return true;
                #else

                if (::fsync(m_file) < 0) {
                    m_out << "inlib::root::file::synchronize :"
                          << " error in fsync() for file " << sout(m_path) << "."
                          << std::endl;
                    return false;
                }

                #endif
                return true;
            }

            virtual bool ziper(char a_key, compress_func& a_func) const
            {
                std::map<char, compress_func>::const_iterator it = m_zipers.find(a_key);

                if (it == m_zipers.end()) {
                    a_func = 0;
                    return false;
                }

                a_func = (*it).second;
                return true;
            }

            virtual bool unziper(char a_key, decompress_func& a_func) const
            {
                std::map<char, decompress_func>::const_iterator it = m_unzipers.find(a_key);

                if (it == m_unzipers.end()) {
                    a_func = 0;
                    return false;
                }

                a_func = (*it).second;
                return true;
            }

            virtual uint32 compression() const
            {
                return m_compress;
            }
            virtual void compress_buffer(const buffer& a_buffer,
                                         char*& a_kbuf, uint32& a_klen, bool& a_kdel)
            {
                //NOTE : if(kdelete) delete [] kbuf;
                a_kbuf = 0;
                a_klen = 0;
                a_kdel = false;
                uint32 nbytes = a_buffer.length();
                uint32 cxlevel = m_compress;

                if (cxlevel && (nbytes > 256)) {
                    inlib::compress_func func;

                    if (!ziper('Z', func)) {
                        //m_out << "inlib::root::directory::write_object :"
                        //      << " zlib ziper not found."
                        //      << std::endl;
                        a_kbuf = (char*)a_buffer.buf();
                        a_klen = a_buffer.length();
                        a_kdel = false;
                    } else {
                        const uint32 kMAXBUF = 0xffffff;
                        const uint32 HDRSIZE = 9;
                        uint32 nbuffers = nbytes / kMAXBUF;
                        uint32 buflen = nbytes + HDRSIZE * (nbuffers + 1);
                        a_kbuf = new char[buflen];
                        a_kdel = true;
                        char* src = (char*)a_buffer.buf();
                        char* tgt = a_kbuf;
                        uint32 nzip = 0;

                        for (uint32 i = 0; i <= nbuffers; i++) {
                            uint32 bufmax = ((i == nbuffers) ? nbytes - nzip : kMAXBUF);
                            uint32 nout;

                            if (!zip(m_out, func, cxlevel, bufmax, src, bufmax, tgt, nout)) {
                                delete [] a_kbuf;
                                a_kbuf = (char*)a_buffer.buf();
                                a_klen = a_buffer.length();
                                a_kdel = false;
                                break;
                            }

                            tgt += nout; //nout includes HDRSIZE
                            a_klen += nout;
                            src += kMAXBUF;
                            nzip += kMAXBUF;
                        }

                        //::printf("debug : compress : end : %u %u\n",nbytes,klen);
                    }
                } else {
                    a_kbuf = (char*)a_buffer.buf();
                    a_klen = a_buffer.length();
                    a_kdel = false;
                }
            }

            virtual key& sinfos_key()
            {
                return m_streamer_info_key;
            }
        public:
            file(std::ostream& a_out, const std::string& a_path, bool a_verbose = false)
                : m_out(a_out)
                , m_path(a_path)
                , m_verbose(a_verbose)
                , m_file(not_open())
                  //,m_bytes_write(0)
                , m_root_directory(*this, nosuffix(a_path), m_title)
                  // begin of record :
                , m_version(0)
                , m_BEGIN(0)
                , m_END(0)
                , m_seek_free(0)
                , m_nbytes_free(0)
                , m_nbytes_name(0)
                , m_units(4)
                , m_compress(1)
                , m_seek_info(0)
                , m_nbytes_info(0)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
                m_version = version();

                if (access_path(m_path, kFileExists)) unlink(m_path);

                if (!m_root_directory.is_valid()) {
                    m_out << "inlib::root::file::file :"
                          << " " << sout(m_path) << " root directory badly created."
                          << std::endl;
                    return;
                }

                m_file = _open(a_path.c_str(),
                               #ifdef _MSC_VER
                               O_RDWR | O_CREAT | O_BINARY, S_IREAD | S_IWRITE
                               #else
                               O_RDWR | O_CREAT, 0644
                               #endif
                              );

                if (m_file == not_open()) {
                    m_out << "inlib::root::file::file :"
                          << " can't open " << sout(a_path) << "."
                          << std::endl;
                    return;
                }

                //initialize :
                m_BEGIN = kBegin();  // First used word in file following the file header.
                m_END = m_BEGIN;   // Pointer to end of file.
                m_free_segs.push_back(new free_seg(m_out, m_BEGIN, START_BIG_FILE()));
                // Write Directory info :
                uint32 namelen =
                    key::std_string_record_size(m_path) +
                    key::std_string_record_size(m_title);
                uint32 nbytes = namelen + m_root_directory.record_size();
                root::key key(*this, 0, m_path, m_title, "TFile", nbytes); //set m_END.
                // m_nbytes_name = start point of directory info from key head.
                m_nbytes_name = key.key_length() + namelen;
                m_root_directory.set_nbytes_name(m_nbytes_name);
                m_root_directory.set_seek_directory(key.seek_key()); //at EOF.

                //the below write 45 bytes at BOF (Begin Of File).
                if (!write_header()) { //need m_nbytes_name, m_END after key written.
                    m_out << "inlib::root::file::file :"
                          << " can't write file header."
                          << std::endl;
                    return;
                }

                {
                    char* pos = key.data_buffer();
                    wbuf wb(m_out, byte_swap(), key.eob(), pos);

                    if (!wb.write(m_path)) return;

                    if (!wb.write(m_title)) return;

                    if (!m_root_directory.to_buffer(wb)) return;
                }

                if (m_verbose) {
                    m_out << "inlib::root::file::file :"
                          << " write key ("
                          << namelen
                          << ", "
                          << m_root_directory.record_size()
                          << ", "
                          << nbytes
                          << ", "
                          << m_nbytes_name
                          << ", "
                          << key.seek_key()
                          << ")."
                          << std::endl;
                }

                key.set_cycle(1);

                if (!key.write_self()) {
                    m_out << "inlib::root::file::file :"
                          << " key.write_self() failed."
                          << std::endl;
                    return;
                }

                //the below write at kBegin + nbytes.
                //64+52
                uint32 n;

                if (!key.write_file(n)) {
                    m_out << "inlib::root::file::file :"
                          << " can't write key in file."
                          << std::endl;
                    return;
                }

                //::printf("debug : file::file : write key : %d\n",n);
            }

            file(std::ostream& a_out, const std::string& a_path, bool a_verbose = false)
                : m_out(a_out)
                , m_path(a_path)
                , m_verbose(a_verbose)
                , m_file(not_open())
                , m_bytes_read(0)
                , m_root_directory(*this)
                , m_streamer_info_key(*this)
                  // begin of record :
                , m_version(0)
                , m_BEGIN(0)
                , m_END(0)
                , m_seek_free(0)
                , m_seek_info(0)
                , m_nbytes_free(0)
                , m_nbytes_info(0)
                , m_nbytes_name(0)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
                m_file = _open(a_path.c_str(),
                               #ifdef _MSC_VER
                               O_RDONLY | O_BINARY, S_IREAD | S_IWRITE
                               #else
                               O_RDONLY, 0644
                               #endif
                              );

                if (m_file == not_open()) {
                    m_out << "inlib::root::file::file :"
                          << " can't open " << sout(a_path) << "."
                          << std::endl;
                    return;
                }

                initialize();
            }

            virtual ~file()
            {
                close();
                #ifdef INLIB_MEM
                mem::decrement(s_class().c_str());
                #endif
            }
        protected:
            file(const file& a_from)
                : ifile(a_from)
                , m_out(a_from.m_out)
                , m_root_directory(*this)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
            }

            file(const file& a_from)
                : ifile(a_from)
                , m_out(a_from.m_out)
                , m_root_directory(*this)
                , m_streamer_info_key(*this)
            {
                #ifdef INLIB_MEM
                mem::increment(s_class().c_str());
                #endif
            }

            file& operator=(const file&)
            {
                return *this;
            }
        public:
            uint32 version() const
            {
                return m_version;
            }

            void set_compression(uint32 a_level)
            {
                // level = 0 objects written to this file will not be compressed.
                // level = 1 minimal compression level but fast.
                // ....
                // level = 9 maximal compression level but slow.
                m_compress = a_level;

                if (m_compress > 9) m_compress = 9;
            }

            bool is_open() const
            {
                return (m_file == not_open() ? false : true);
            }

            void close()
            {
                if (m_file == not_open()) return;

                if (m_free_segs.size()) {
                    if (!write_free_segments()) {
                        m_out << "inlib::root::file::close :"
                              << " can't write free segments."
                              << std::endl;
                    }

                    if (!write_header())  { // Now write file header
                        m_out << "inlib::root::file::close :"
                              << " can't write file header."
                              << std::endl;
                    }
                }

                {
                    std::list<free_seg*>::iterator it;

                    for (it = m_free_segs.begin();
                         it != m_free_segs.end();
                         it = m_free_segs.erase(it)) {
                        delete(*it);
                    }
                }

                m_root_directory.clear_keys();
                m_root_directory.close();
                ::close(m_file);
                m_file = not_open();
            }

            directory& dir()
            {
                return m_root_directory;
            }
            const directory& dir() const
            {
                return m_root_directory;
            }

            bool write(uint32& a_nbytes)
            {
                // Write memory objects to this file :
                //  Loop on all objects in m_root_directory (including subdirectories).
                //  A new key is created in the directories m_keys linked list
                //  for each object.
                //  The list of keys is then saved on the file (via write_keys)
                //  as a single data record.
                //  The directory header info is rewritten on the directory header record.
                //  //The linked list of FREE segments is written.
                //  The file header is written (bytes 1->m_BEGIN).
                a_nbytes = 0;

                if (m_verbose) {
                    m_out << "inlib::root::file::write :"
                          << " writing Name=" << sout(m_path)
                          << " Title=" << sout(m_title) << "."
                          << std::endl;
                }

                uint32 nbytes;

                if (!m_root_directory.write(nbytes)) return false; // Write directory tree

                if (!write_streamer_infos()) {
                    m_out << "inlib::root::file::write :"
                          << " write_streamer_infos failed."
                          << std::endl;
                    return false;
                }

                if (!write_free_segments()) {
                    m_out << "inlib::root::file::write :"
                          << " can't write free segments."
                          << std::endl;
                    return false;
                }

                if (!write_header()) { //write 45 bytes at BOF.
                    m_out << "inlib::root::file::write :"
                          << " can't write file header."
                          << std::endl;
                    return false;
                }

                a_nbytes = nbytes;
                return true;
            }

            bool add_unziper(char a_key, decompress_func a_func)
            {
                std::map<char, decompress_func>::const_iterator it = m_unzipers.find(a_key);

                if (it != m_unzipers.end()) {
                    //(*it).second = a_func; //override ?
                    return false;
                } else {
                    m_unzipers[a_key] = a_func;
                    return true;
                }
            }

            bool add_ziper(char a_key, compress_func a_func)
            {
                std::map<char, compress_func>::const_iterator it = m_zipers.find(a_key);

                if (it != m_zipers.end()) {
                    //(*it).second = a_func; //override ?
                    return false;
                } else {
                    m_zipers[a_key] = a_func;
                    return true;
                }
            }
        protected:
            enum EAccessMode {
                kFileExists        = 0,
                kExecutePermission = 1,
                kWritePermission   = 2,
                kReadPermission    = 4
            };
            static int _open(const char* a_name, int a_flags, uint32 a_mode)
            {
                #if defined(__linux__) && (__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 2)
                return ::open64(a_name, a_flags, a_mode);
                #else
                return ::open(a_name, a_flags, a_mode);
                #endif
            }

            static int _open(const char* a_name, int a_flags, unsigned int a_mode)
            {
                #if defined(__linux__) && (__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 2)
                return ::open64(a_name, a_flags, a_mode);
                #else
                return ::open(a_name, a_flags, a_mode);
                #endif
            }

            static std::string sout(const std::string& a_string)
            {
                return "\"" + a_string + "\"";
            }
            static bool access_path(const std::string& a_path, EAccessMode a_mode)
            {
                // Returns true if one can access a file using the specified access mode.
                // Mode is the same as for the WinNT access(2) function.
                #ifdef _MSC_VER
                return (::_access(a_path.c_str(), a_mode) == 0) ? true : false;
                #else
                return (::access(a_path.c_str(), a_mode) == 0) ? true : false;
                #endif
            }
            static bool unlink(const std::string& a_path)
            {
                // Unlink, i.e. remove, a file or directory. Returns true when succesfull,
                // false in case of failure.
                struct stat finfo;

                if (::stat(a_path.c_str(), &finfo) < 0) return false;

                #ifdef _MSC_VER

                if (finfo.st_mode & S_IFDIR)
                    return (::_rmdir(a_path.c_str()) == -1 ? false : true);
                else
                    return (::unlink(a_path.c_str()) == -1 ? false : true);

                #else

                if (S_ISDIR(finfo.st_mode))
                    return (::rmdir(a_path.c_str()) == -1 ? false : true);
                else
                    return (::unlink(a_path.c_str()) == -1 ? false : true);

                #endif
            }

            bool initialize()
            {
                if (!read_header()) {
                    m_out << "inlib::root::file::initialize :"
                          << " can't read header."
                          << std::endl;
                    return false;
                }

                /*
                    fRootDirectory->setSeekDirectory(fBEGIN);
                    // Read Free segments structure if file is writable :
                    if (fWritable) {
                      if (fSeekFree > fBEGIN) {
                        if(!readFreeSegments()) {
                          m_out << "inlib::root::file::initialize : Cannot read free segments."
                               << std::endl;
                          return false;
                        }
                      } else {
                        m_out << "inlib::root::file::initialize : file \"" << fName
                             << "\" probably not closed, cannot read free segments" << std::endl;
                      }
                    }
                */
                // Read Directory info :
                uint32 nbytes = m_nbytes_name + m_root_directory.record_size(m_version);
                char* header = new char[nbytes];
                char* buffer = header;

                if (!set_pos(m_BEGIN)) {
                    m_out << "inlib::root::file::initialize :"
                          << " can't set position."
                          << std::endl;
                    delete [] header;
                    return false;
                }

                if (!read_buffer(buffer, nbytes)) {
                    m_out << "inlib::root::file::initialize :"
                          << " can't read buffer."
                          << std::endl;
                    delete [] header;
                    return false;
                }

                buffer = header + m_nbytes_name;
                const char* eob = header + nbytes;

                if (!m_root_directory.from_buffer(eob, buffer)) {
                    m_out << "inlib::root::file::initialize :"
                          << " can't read buffer (2)."
                          << std::endl;
                    delete [] header;
                    return false;
                }

                uint32 nk =          //size of Key
                    sizeof(int) +      //Key::fNumberOfBytes
                    sizeof(short) +    //Key::fVersion
                    2 * sizeof(int) +  //Key::fObjectSize, Date
                    2 * sizeof(short) + //Key::fKeyLength,fCycle
                    2 * sizeof(seek32); //Key::fSeekKey,fSeekParentDirectory
                //WARNING : the upper is seek32 since at begin of file.
                buffer = header + nk;
                std::string cname;
                rbuf rb(m_out, byte_swap(), eob, buffer);

                // Should be "TFile".
                if (!rb.read(cname)) {
                    m_out << "inlib::root::file::initialize :"
                          << " can't read buffer (3)."
                          << std::endl;
                    delete [] header;
                    return false;
                }

                if (cname != "TFile") {
                    m_out << "inlib::root::file::initialize : TFile expected." << std::endl;
                    delete [] header;
                    return false;
                }

                if (m_verbose) {
                    m_out << "inlib::root::file::initialize :"
                          << " " << sout("TFile") << " found."
                          << std::endl;
                }

                if (!rb.read(cname)) {
                    m_out << "inlib::root::file::initialize :"
                          << " can't read buffer (4)."
                          << std::endl;
                    delete [] header;
                    return false;
                }

                if (m_verbose) {
                    m_out << "inlib::root::file::initialize :"
                          << " found file name " << sout(cname)
                          << std::endl;
                }

                if (!rb.read(m_title)) {
                    m_out << "inlib::root::file::initialize :"
                          << " can't read buffer (5)."
                          << std::endl;
                    delete [] header;
                    return false;
                }

                delete [] header;

                if (m_verbose) {
                    m_out << "inlib::root::file::initialize :"
                          << " found title " << sout(m_title)
                          << std::endl;
                }

                uint32 dirNbytesName = m_root_directory.nbytes_name();

                if (dirNbytesName < 10 || dirNbytesName > 1000) {
                    m_out << "inlib::root::file::initialize :"
                          << " can't read directory info."
                          << std::endl;
                    return false;
                }

                // Read keys of the top directory :
                if (m_root_directory.seek_keys() > m_BEGIN) {
                    uint32 n;

                    if (!m_root_directory.read_keys(n)) {
                        m_out << "inlib::root::file::initialize :"
                              << " can't read keys."
                              << std::endl;
                        return false;
                    }
                } else {
                    m_out << "inlib::root::file::initialize :"
                          << " file " << sout(m_path)
                          << " probably not closed."
                          << std::endl;
                    return false;
                }

                // Create StreamerInfo index
                if (m_seek_info > m_BEGIN) {
                    if (!read_streamer_infos()) {
                        m_out << "inlib::root::file::initialize :"
                              << " read_streamer_infos() failed."
                              << std::endl;
                        return false;
                    }
                } else {
                    m_out << "inlib::root::file::initialize :"
                          << " file " << sout(m_path)
                          << " probably not closed."
                          << std::endl;
                    return false;
                }

                return true;
            }

            bool read_header()
            {
                static const uint32 kBegin = 64;
                char header[kBegin];

                if (!set_pos()) return false;

                if (!read_buffer(header, kBegin)) return false;

                // make sure this is a root file
                if (::strncmp(header, "root", 4)) {
                    m_out << "inlib::root::file::read_header :"
                          << " " << sout(m_path) << " not a file at the CERN-ROOT format."
                          << std::endl;
                    return false;
                }

                if (m_verbose) {
                    m_out << "inlib::root::file::read_header :"
                          << " file signature is " << sout("root")
                          << std::endl;
                }

                char* buffer = header + 4;    // skip the "root" file identifier
                const char* eob = header + kBegin;
                rbuf rb(m_out, byte_swap(), eob, buffer);
                {
                    int v;

                    if (!rb.read(v)) return false;

                    m_version = v;
                }
                {
                    seek32 i;

                    if (!rb.read(i)) return false;

                    m_BEGIN = i;
                }

                if (m_version > 1000000) {
                    if (!rb.read(m_END)) return false;

                    if (!rb.read(m_seek_free)) return false;
                } else {
                    {
                        seek32 i;

                        if (!rb.read(i)) return false;

                        m_END = i;
                    }
                    {
                        seek32 i;

                        if (!rb.read(i)) return false;

                        m_seek_free = i;
                    }
                }

                if (m_verbose) {
                    m_out << "inlib::root::file::read_header :"
                          << " begin " << m_BEGIN
                          << " end " << m_END
                          << std::endl;
                }

                {
                    int v;

                    if (!rb.read(v)) return false;

                    m_nbytes_free = v;
                }

                int nfree = 0;

                if (!rb.read(nfree)) return false;

                {
                    int v;

                    if (!rb.read(v)) return false;

                    m_nbytes_name = v;
                }
                //m_out << "debug : 1002 " << m_nbytes_name << std::endl;
                {
                    char fUnits;

                    if (!rb.read(fUnits)) return false;
                }
                {
                    int fCompress;

                    if (!rb.read(fCompress)) return false;
                }

                if (m_version > 1000000) {
                    if (!rb.read(m_seek_info)) return false;
                } else {
                    {
                        seek32 i;

                        if (!rb.read(i)) return false;

                        m_seek_info = i;
                    }
                }

                if (!rb.read(m_nbytes_info)) return false;

                //m_out << "debug : seek_info " << m_seek_info << " nbytes_info " << m_nbytes_info << std::endl;
                return true;
            }

            bool write_header()
            {
                const char root[] = "root";
                //char psave[kBegin()];
                char psave[128];
                const char* eob = psave + kBegin();
                char* pos = psave;
                ::memcpy(pos, root, 4);
                pos += 4;
                uint32 vers = m_version;

                if ((m_END > START_BIG_FILE())        ||
                    (m_seek_free > START_BIG_FILE())  ||
                    (m_seek_info > START_BIG_FILE())) {
                    vers += 1000000;
                    m_units = 8;
                }

                wbuf wb(m_out, byte_swap(), eob, pos);

                if (!wb.write(vers)) return false;

                if (!wb.write((seek32)m_BEGIN)) return false;

                if (vers > 1000000) {
                    if (!wb.write(m_END)) return false;

                    if (!wb.write(m_seek_free)) return false;
                } else {
                    if (!wb.write((seek32)m_END)) return false;

                    if (!wb.write((seek32)m_seek_free)) return false;
                }

                if (!wb.write(m_nbytes_free)) return false;

                //int nfree  = fFreeSegments.size();
                uint32 nfree  = 0; //FIXME

                if (!wb.write(nfree)) return false;

                if (!wb.write(m_nbytes_name)) return false;

                if (!wb.write(m_units)) return false;

                if (!wb.write(m_compress)) return false;

                if (vers > 1000000) {
                    if (!wb.write(m_seek_info)) return false;
                } else {
                    if (!wb.write((seek32)m_seek_info)) return false;
                }

                if (!wb.write(m_nbytes_info)) return false;

                if (!set_pos()) return false; //BOF

                uint32 nbytes = uint32(pos - psave);

                //::printf("debug : write_header : %d\n",nbytes);
                if (!write_buffer(psave, nbytes)) return false;

                if (!synchronize()) return false;

                return true;
            }

            bool read_streamer_infos()
            {
                // Read the list of StreamerInfo from this file
                // The key with name holding the list of TStreamerInfo objects is read.
                // The corresponding TClass objects are updated.
                if (m_seek_info <= 0) return false;

                if (m_seek_info >= m_END) return false;

                if (!set_pos(m_seek_info)) return false;

                char* buffer = new char[m_nbytes_info + 1];

                if (!read_buffer(buffer, m_nbytes_info)) {
                    delete [] buffer;
                    return false;
                }

                char* buf = buffer;

                if (!m_streamer_info_key.from_buffer(buffer + m_nbytes_info, buf)) {
                    delete [] buffer;
                    return false;
                }

                delete [] buffer;
                return true;
            }

            bool write_streamer_infos()
            {
                List<StreamerInfo> sinfos;
                fill_infos(sinfos, m_out);

                if (sinfos.empty()) return false;

                buffer bref(m_out, byte_swap(), 256);

                if (!sinfos.stream(bref)) {
                    m_out << "inlib::root::file::write_streamer_infos :"
                          << " cannot stream List<StreamerInfo>."
                          << std::endl;
                    return false;
                }

                uint32 nbytes = bref.length();
                root::key key(*this,
                              m_root_directory.seek_directory(),
                              "StreamerInfo", "",
                              sinfos.store_cls(),
                              nbytes); //set m_END

                if (!key.seek_key()) return false;

                if (!bref.displace_mapped(key.key_length())) return false;

                ::memcpy(key.data_buffer(), bref.buf(), nbytes);

                //key.set_cycle(1);
                if (!key.write_self()) {
                    m_out << "inlib::root::file::write_streamer_infos :"
                          << " key.write_self() failed."
                          << std::endl;
                    return false;
                }

                m_seek_info = key.seek_key();
                m_nbytes_info = key.number_of_bytes();
                //FIXME sumBuffer(key.objectSize());
                uint32 n;

                if (!key.write_file(n)) return false;

                if (!n) return false;

                return true;
            }

            bool make_free_seg(seek a_first, seek a_last)
            {
                // Mark unused bytes on the file :
                //  The list of free segments is in the m_free_segs list
                //  When an object is deleted from the file, the freed space is added
                //  into the FREE linked list (m_free_segs). The FREE list consists
                //  of a chain  of consecutive free segments on the file. At the same
                //  time, the first 4 bytes of the freed record on the file
                //  are overwritten by GAPSIZE where
                //    GAPSIZE = -(Number of bytes occupied by the record).
                if (m_free_segs.empty()) {
                    m_out << "inlib::root::file::make_free_seg :"
                          << " free_seg list should not be empty here."
                          << std::endl;
                    return false;
                }

                free_seg* newfree = add_free(m_free_segs, a_first, a_last);

                if (!newfree) {
                    m_out << "inlib::root::file::make_free_seg :"
                          << " add_free failed."
                          << std::endl;
                    return false;
                }

                seek nfirst = newfree->first();
                seek nlast = newfree->last();
                seek _nbytes = nlast - nfirst + 1;

                if (_nbytes > START_BIG_FILE()) _nbytes = START_BIG_FILE();

                int nbytes = -int(_nbytes);
                int nb = sizeof(int);
                char psave[128];
                const char* eob = psave + nb;
                char* pos = psave;
                wbuf wb(m_out, byte_swap(), eob, pos);

                if (!wb.write(nbytes)) return false;

                if (nlast == (m_END - 1)) m_END = nfirst;

                if (!set_pos(nfirst)) return false;

                if (!write_buffer(psave, nb)) return false;

                if (!synchronize()) return false;

                return true;
            }

            bool write_free_segments()
            {
                //  The linked list of FREE segments (fFree) is written as a single data
                //  record.

                // Delete old record if it exists :
                if (m_seek_free) {
                    if (!make_free_seg(m_seek_free, m_seek_free + m_nbytes_free - 1)) {
                        m_out << "inlib::root::file::write_free_segments :"
                              << " key.write_self() failed."
                              << std::endl;
                        return false;
                    }
                }

                //::printf("debug : write_free_segments : seg list :\n");
                uint32 nbytes = 0;
                {
                    inlib_lforcit(free_seg*, m_free_segs, it) {
                        nbytes += (*it)->record_size();
                        //::printf("debug : write_free_segments : %lu %lu\n",
                        //         (*it)->first(),(*it)->last());
                    }
                }

                if (!nbytes) return true;

                root::key key(*this,
                              m_root_directory.seek_directory(),
                              m_path, m_title, "TFile",
                              nbytes); //set m_END

                if (!key.seek_key()) return false;

                {
                    char* pos = key.data_buffer();
                    wbuf wb(m_out, byte_swap(), key.eob(), pos);
                    inlib_lforcit(free_seg*, m_free_segs, it) {
                        if (!(*it)->fill_buffer(wb)) return false;
                    }
                }

                //key.set_cycle(1);
                if (!key.write_self()) {
                    m_out << "inlib::root::file::write_free_segments :"
                          << " key.write_self() failed."
                          << std::endl;
                    return false;
                }

                m_seek_free = key.seek_key();
                m_nbytes_free = key.number_of_bytes();

                if (m_verbose) {
                    m_out << "inlib::root::file::write_free_segments :"
                          << " write key." << std::endl;
                }

                uint32 n;

                if (!key.write_file(n)) return false;

                if (!n) return false;

                return true;
            }

            static bool zip(std::ostream& a_out,
                            inlib::compress_func a_func,
                            int a_level,
                            uint32 a_srcsize, char* a_src,
                            uint32 a_tgtsize, char* a_tgt,
                            uint32& a_irep)
            {
                // from Rio/Bits/R__zip using zlib.
                const uint32 HDRSIZE = 9;

                if (a_tgtsize < HDRSIZE) {
                    a_out << "inlib::root::directory::zip :"
                          << " target buffer too small."
                          << std::endl;
                    a_irep = 0;
                    return false;
                }

                if (a_srcsize > 0xffffff) {
                    a_out << "inlib::root::directory::zip :"
                          << " source buffer too big."
                          << std::endl;
                    a_irep = 0;
                    return false;
                }

                uint32 out_size;

                if (!a_func(a_out, a_level,
                            a_srcsize, a_src,
                            a_tgtsize, a_tgt + HDRSIZE,
                            out_size)) {
                    a_out << "inlib::root::directory::zip :"
                          << " zipper failed."
                          << std::endl;
                    a_irep = 0;
                    return false;
                }

                if ((HDRSIZE + out_size) > a_tgtsize) {
                    a_out << "inlib::root::directory::zip :"
                          << " target buffer overflow."
                          << std::endl;
                    a_irep = 0;
                    return false;
                }

                // HEADER :
                a_tgt[0] = 'Z'; // Signature ZLib
                a_tgt[1] = 'L';
                a_tgt[2] = 8; //DEFLATE
                a_tgt[3] = (char)(out_size & 0xff);
                a_tgt[4] = (char)((out_size >> 8) & 0xff);
                a_tgt[5] = (char)((out_size >> 16) & 0xff);
                a_tgt[6] = (char)(a_srcsize & 0xff);
                a_tgt[7] = (char)((a_srcsize >> 8) & 0xff);
                a_tgt[8] = (char)((a_srcsize >> 16) & 0xff);
                a_irep = HDRSIZE + out_size;
                return true;
            }

            #if defined(__sun) && !defined(__linux__) && (__SUNPRO_CC > 0x420)
            int error_number()
            {
                return ::errno;
            }
            void reset_error_number()
            {
                ::errno = 0;
            }
            #else
            int error_number()
            {
                return errno;
            }
            void reset_error_number()
            {
                errno = 0;
            }
            #endif

        protected:
            std::ostream& m_out;
            std::string m_path;
            bool m_verbose;
            int m_file;
            inlib::uint64 m_bytes_read; //Number of bytes read from this file
            //uint64 m_bytes_write; //Number of bytes write in this file
            std::string m_title; //must be before the below.
            directory m_root_directory;
            key m_streamer_info_key;
            std::map<char, decompress_func> m_unzipers;
            std::map<char, compress_func> m_zipers;
            std::list<free_seg*> m_free_segs; //Free segments linked list table
            // begin of record :
            // "root"
            uint32 m_version;       //File format version
            seek m_BEGIN;           //First used byte in file
            seek m_END;             //Last used byte in file
            seek m_seek_free;       //Location on disk of free segments structure
            uint32 m_nbytes_free;   //Number of bytes for free segments structure
            //int nfree
            uint32 m_nbytes_name;   //Number of bytes in TNamed at creation time
            char m_units;           //Number of bytes for file pointers
            uint32 m_compress;      //(=1 file is compressed, 0 otherwise)
            seek m_seek_info;       //Location on disk of StreamerInfo record
            uint32 m_nbytes_info;   //Number of bytes for StreamerInfo record
        };


    }
}

//inlib_build_use kernel

#endif

//doc
//
//  A ROOT file is a suite of consecutive data records with the following
//    format (see also the TKey class);
// TKey ---------------------
//      byte 1->4  Nbytes    = Length of compressed object (in bytes)
//           5->6  Version   = TKey version identifier
//           7->10 ObjLen    = Length of uncompressed object
//          11->14 Datime    = Date and time when object was written to file
//          15->16 KeyLen    = Length of the key structure (in bytes)
//          17->18 Cycle     = Cycle of key
//          19->22 SeekKey   = Pointer to record itself (consistency check)
//          23->26 SeekPdir  = Pointer to directory header
//          27->27 lname     = Number of bytes in the class name
//          28->.. ClassName = Object Class Name
//          ..->.. lname     = Number of bytes in the object name
//          ..->.. Name      = lName bytes with the name of the object
//          ..->.. lTitle    = Number of bytes in the object title
//          ..->.. Title     = Title of the object
//          -----> DATA      = Data bytes associated to the object
//
//  The first data record starts at byte fBEGIN (currently set to kBegin)
//  Bytes 1->kBegin contain the file description:
//       byte  1->4  "root"      = Root file identifier
//             5->8  fVersion    = File format version
//             9->12 fBEGIN      = Pointer to first data record
//            13->16 fEND        = Pointer to first free word at the EOF
//            17->20 fSeekFree   = Pointer to FREE data record
//            21->24 fNbytesFree = Number of bytes in FREE data record
//            25->28 nfree       = Number of free data records
//            29->32 fNbytesName = Number of bytes in TNamed at creation time
//            33->33 fUnits      = Number of bytes for file pointers
//            34->37 fCompress   = Zip compression level
//
