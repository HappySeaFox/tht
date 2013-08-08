TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = lib
lib.file = THT-lib.pro

# find plugins
PROS = $$system(dir /ON /B "THT-plugin-*.pro" 2>nul)

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
