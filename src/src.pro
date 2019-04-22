TEMPLATE = subdirs
SUBDIRS += \
    wifi \
    imports

imports.depends = wifi

