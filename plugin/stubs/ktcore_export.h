#ifndef KTCORE_EXPORT_H
#define KTCORE_EXPORT_H
#define KTCORE_EXPORT __attribute__((visibility("default")))
#define KTCORE_NO_EXPORT __attribute__((visibility("hidden")))
#endif
