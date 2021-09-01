# opengl-automaskobj

## Install
```bash
conda create -n coco python=3.8 -y
conda activate coco

pip install tqdm cython future tensorboard
pip install git+git://github.com/waspinator/coco.git@2.1.0
pip install git+git://github.com/waspinator/pycococreator.git@0.2.0
pip install "git+https://github.com/cocodataset/cocoapi.git#subdirectory=PythonAPI"
```

## Prepare Dataset
edit shapes_to_coco.py
```bash
python shapes_to_coco.py
```
