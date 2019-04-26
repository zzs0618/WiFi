TEMPLATE = subdirs
SUBDIRS += \
    cmake
!linux-oe-g++ {
    SUBDIRS += unit
}
