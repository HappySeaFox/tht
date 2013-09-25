TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = lib
lib.file = THT-lib.pro

# find plugins
PROS = $$files(THT-plugin-*.pro)

for(pro, PROS) {
    name = $$replace(pro, \\., _)
    name = $$replace(name, -, _)
    SUBDIRS += $${name}
    eval($${name}.file = $$pro)
    eval($${name}.depends = lib)
}

SUBDIRS += app
app.file = THT-app.pro
app.depends = lib
