#!/usr/bin/env python3

import datetime
import json
import os
import re
import fnmatch
from PIL import Image
import numpy as np
from pycococreatortools import pycococreatortools

class ShapesToCOCO:
    def __init__(self):
        self.reset()
        return

    def reset(self):
        self.dataset_root_dir = None
        self.data_type_list = []
        self.info = {
            "description": "Example Dataset",
            "url": "https://github.com/waspinator/pycococreator",
            "version": "0.1.0",
            "year": 2018,
            "contributor": "waspinator",
            "date_created": datetime.datetime.utcnow().isoformat(' ')}
        self.licenses = [{
            "id": 1,
            "name": "Attribution-NonCommercial-ShareAlike License",
            "url": "http://creativecommons.org/licenses/by-nc-sa/2.0/"}]
        self.classes = []
        self.categories = []
        return

    def setDatasetRootDir(self, dataset_root_dir):
        self.dataset_root_dir = dataset_root_dir
        if self.dataset_root_dir[-1] != "/":
            self.dataset_root_dir += "/"
        return

    def setDataType(self, data_type_list):
        self.data_type_list = data_type_list
        return

    def createCategories(self):
        for i in range(len(classes)):
            current_dict = {'id':i+1, 'name':classes[i], 'supercategory':'shape'}
            self.categories.append(current_dict)
        return

    def setClasses(self, classes):
        self.classes = classes
        self.createCategories()
        return

    def filter_for_jpeg(self, root, files):
        file_types = ['*.jpg']
        file_types = r'|'.join([fnmatch.translate(x) for x in file_types])
        files = [os.path.join(root, f) for f in files]
        files = [f for f in files if re.match(file_types, f)]
        
        return files

    def filter_for_annotations(self, root, files, image_filename):
        file_types = ['*.jpg']
        file_types = r'|'.join([fnmatch.translate(x) for x in file_types])
        basename_no_extension = os.path.splitext(os.path.basename(image_filename))[0]
        file_name_prefix = basename_no_extension + '.*'
        files = [os.path.join(root, f) for f in files]
        files = [f for f in files if re.match(file_types, f)]
        files = [f for f in files if re.match(file_name_prefix, os.path.splitext(os.path.basename(f))[0])]

        return files

    def createCOCOJson(self):

        for data_type in data_type_list:
            coco_output = {
                "info": self.info,
                "licenses": self.licenses,
                "categories": self.categories,
                "images": [],
                "annotations": []
            }

            image_id = 1
            segmentation_id = 1

            image_dir = self.dataset_root_dir + data_type + "/images/"
            annotation_dir = self.dataset_root_dir + data_type + "/annotations/"

            
            # filter for jpeg images
            for root, _, files in os.walk(image_dir):
                image_files = self.filter_for_jpeg(root, files)

                solved_image_num = 0

                # go through each image
                for image_filename in image_files:
                    solved_image_num += 1

                    image = Image.open(image_filename)

                    image_info = pycococreatortools.create_image_info(
                        image_id, os.path.basename(image_filename), image.size)

                    coco_output["images"].append(image_info)

                    # filter for associated png annotations
                    for root, _, files in os.walk(annotation_dir):
                        annotation_files = self.filter_for_annotations(root, files, image_filename)

                        solved_annotation_num = 0

                        # go through each associated annotation
                        for annotation_filename in annotation_files:
                            solved_annotation_num += 1
                            
                            class_id = [x['id'] for x in self.categories if x['name'] in annotation_filename][0]

                            category_info = {'id': class_id, 'is_crowd': 'crowd' in image_filename}

                            binary_mask = np.asarray(Image.open(annotation_filename)
                                .convert('1')).astype(np.uint8)
                            
                            annotation_info = pycococreatortools.create_annotation_info(
                                segmentation_id, image_id, category_info, binary_mask,
                                image.size, tolerance=2)

                            if annotation_info is not None:
                                coco_output["annotations"].append(annotation_info)

                            segmentation_id = segmentation_id + 1

                            print("\rSolving at : Data type : " + data_type +
                                  " Image : " + str(solved_image_num) + "/" + str(len(image_files)) +
                                  " Annotation : " + str(solved_annotation_num) + "/" + str(len(annotation_files)) +
                                  "    ", end="")

                    image_id += 1
            print()

            with open(self.dataset_root_dir + data_type + '/instances_shape_' + data_type + '.json', 'w') as output_json_file:
                json.dump(coco_output, output_json_file)


if __name__ == "__main__":
    dataset_root_dir = '/home/chli/3D_FRONT/output_mask_1920x1080/'
    classes = ['accessory', 'appliance', 'art', 'basin', 'bath',
               'bed', 'build element', 'cabinet', 'chair', 'electronics',
               'kitchen cabinet', 'lighting', 'media unit', 'mirror', 'outdoor furniture',
               'plants', 'recreation', 'shelf', 'sofa', 'stair',
               'storage unit', 'table', 'wardrobe']
    data_type_list = ['train', 'test', 'val']

    shapes_to_coco = ShapesToCOCO()

    shapes_to_coco.setDatasetRootDir(dataset_root_dir)
    shapes_to_coco.setClasses(classes)
    shapes_to_coco.setDataType(data_type_list)

    shapes_to_coco.createCOCOJson()

