#pragma once

#ifndef _DISABLELIBARCHIVE_

#include "logfile.hh"

#include <archive.h>
#include <archive_entry.h>
#include <cstring>
#include <exception>
#include <mutex>
#include <string>

void extract_tar(const unsigned char *tar_data, size_t tar_size, const char *output_dir)
{
    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS;

    a = archive_read_new();
    archive_read_support_format_tar(a);
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);

    if (archive_read_open_memory(a, tar_data, tar_size) != ARCHIVE_OK)
    {
        fprintf(stderr, "Could not open tar archive: %s\n", archive_error_string(a));
        _PRINTINPUT_2_("Could not open tar archive: " << archive_error_string(a))
        return;
    }

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    {
        const char *currentFile = archive_entry_pathname(entry);
        std::string fullOutputPath = std::string(output_dir) + "/" + currentFile;
        archive_entry_set_pathname(entry, fullOutputPath.c_str());

        int r = archive_write_header(ext, entry);
        if (r != ARCHIVE_OK)
        {
            fprintf(stderr, "Could not write header: %s\n", archive_error_string(ext));
        } else {
            const void *buff;
            size_t size;
            la_int64_t offset;

            while (archive_read_data_block(a, &buff, &size, &offset) == ARCHIVE_OK)
            {
                archive_write_data_block(ext, buff, size, offset);
            }
        }
        archive_write_finish_entry(ext);
    }

    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
}

#else
    #include <stdexcept>

    void extract_tar(const unsigned char *tar_data, size_t tar_size, const char *output_dir)
    {
        throw std::exception("Compiled without libarchive!");
    }
#endif
