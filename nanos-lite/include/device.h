#ifndef DEVICE_H
#define DEVICE_H

#include <common.h>

size_t serial_write(const void *buf, size_t offset, size_t len);

size_t dispinfo_read(void *buf, size_t offset, size_t len);

size_t events_read(void *buf, size_t offset, size_t len);

size_t fb_write(const void *buf, size_t offset, size_t len);

size_t sbctrl_read(void *buf, size_t offset, size_t len);

size_t sbctrl_write(const void *buf, size_t offset, size_t len);

size_t sb_write(const void *buf, size_t offset, size_t len);

#endif