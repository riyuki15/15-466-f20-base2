all : \
    ../dist/tree.pnct \
    ../dist/tree.scene \

../dist/tree.pnct : tree.blend export-meshes.py
	blender --background --python export-meshes.py -- "tree.blend":Main "../dist/tree.pnct"

../dist/tree.scene : tree.blend export-scene.py
	blender --background --python export-scene.py -- "tree.blend":Main "../dist/tree.scene"


