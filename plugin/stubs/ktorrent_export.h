#ifndef KTORRENT_EXPORT_H
#define KTORRENT_EXPORT_H
#define KTORRENT_EXPORT __attribute__((visibility("default")))
#define KTORRENT_NO_EXPORT __attribute__((visibility("hidden")))
#define KTORRENT_DEPRECATED __attribute__((__deprecated__))
#endif
