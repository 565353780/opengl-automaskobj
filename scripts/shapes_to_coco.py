#!/usr/bin/env python3

import datetime
import json
import os
import re
import fnmatch
from PIL import Image
import numpy as np
from pycococreatortools import pycococreatortools
from multiprocessing import Pool, cpu_count
from tqdm import tqdm

class ShapesToCOCO:
    def __init__(self):
        self.reset()
        return

    def reset(self):
        self.dataset_root_dir = None
        self.image_dir = None
        self.annotation_dir = None
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
        self.coco_output = None
        # [[image_id, image_file_path, image_size], ...]
        self.image_info_list = []
        # [[image_id, image_file_path, image_size, annotation_id, annotation_file_path], ...]
        self.annotation_info_list = []
        return

    def poolTask(self, function, task_list, thread_num):
        result_list = []

        pool = Pool(thread_num)

        with tqdm(total=len(task_list)) as t:
            for result in pool.imap(function, task_list):
                if result is not None:
                    result_list.append(result)
                t.update()

        pool.close()
        pool.join()
        return result_list

    def setDatasetRootDir(self, dataset_root_dir):
        self.dataset_root_dir = dataset_root_dir
        if self.dataset_root_dir[-1] != "/":
            self.dataset_root_dir += "/"
        return

    def setDataTypes(self, data_type_list):
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

    def getImageAndSegmentationIDList(self):
        self.image_info_list = []
        self.annotation_info_list = []

        image_file_name_list = os.listdir(self.image_dir)
        annotation_file_name_list = os.listdir(self.annotation_dir)

        for i in range(len(image_file_name_list)):
            image_file_name = image_file_name_list[i]
            image_file_path = self.image_dir + image_file_name

            image = Image.open(image_file_path)
            image_size = image.size

            self.image_info_list.append([i + 1, image_file_path, image_size])

        for i in range(len(annotation_file_name_list)):
            annotation_file_name = annotation_file_name_list[i]
            annotation_image_basename = annotation_file_name.split("_")[0]
            annotation_id = i + 1
            annotation_file_path = self.annotation_dir + annotation_file_name

            for image_info in self.image_info_list:
                image_file_path = image_info[1]
                image_basename = os.path.basename(image_file_path).split(".")[0]
                if image_basename == annotation_image_basename:
                    image_id = image_info[0]
                    image_size = image_info[2]
                    self.annotation_info_list.append(
                        [image_id, image_file_path, image_size, annotation_id, annotation_file_path])
                    break

            print("\rPrepare annotation info list : " +
                  str(i) + "/" + str(len(annotation_file_name_list)) + "    ", end="")
        print()
        return

    def createImageInfoJson(self, image_info):
        image_id = image_info[0]
        image_file_path = image_info[1]
        image_size = image_info[2]
        image_basename = os.path.basename(image_file_path)

        image_info = pycococreatortools.create_image_info(
            image_id, image_basename, image_size)

        #  self.coco_output["images"].append(image_info)
        return image_info

    def createAnnotationInfoJson(self, annotation_info):
        image_id = annotation_info[0]
        image_file_path = annotation_info[1]
        image_size = annotation_info[2]
        annotation_id = annotation_info[3]
        annotation_file_path = annotation_info[4]

        class_id = [x['id'] for x in self.categories if x['name'] in annotation_file_path][0]

        category_info = {'id': class_id, 'is_crowd': 'crowd' in image_file_path}

        binary_mask = np.asarray(Image.open(annotation_file_path)
            .convert('1')).astype(np.uint8)
        
        annotation_info = pycococreatortools.create_annotation_info(
            annotation_id, image_id, category_info, binary_mask,
            image_size, tolerance=2)

        #  if annotation_info is not None:
        #      self.coco_output["annotations"].append(annotation_info)
        return annotation_info

    def createCOCOJson(self, thread_num):

        for data_type in data_type_list:
            self.coco_output = {
                "info": self.info,
                "licenses": self.licenses,
                "categories": self.categories,
                "images": [],
                "annotations": []
            }

            self.image_dir = self.dataset_root_dir + data_type + "/images/"
            self.annotation_dir = self.dataset_root_dir + data_type + "/annotations/"

            print("Start createCOCOJson for " + data_type + " dataset...")
            print("Start prepare annotation info list...")

            self.getImageAndSegmentationIDList()

            print("Finish prepare annotation info list!")
            print("Start create image info json with " + str(thread_num) + "threads...")

            image_info_list = \
                self.poolTask(self.createImageInfoJson, self.image_info_list, thread_num)

            self.coco_output["images"] = image_info_list

            print("Finish create image info json!")
            print("Start create annotation info json with " + str(thread_num) + "threads...")

            annotation_info_list = \
                self.poolTask(self.createAnnotationInfoJson, self.annotation_info_list, thread_num)

            self.coco_output["annotations"] = annotation_info_list

            print("Finish create annotation info json!")

            with open(self.dataset_root_dir + data_type + '/test_instances_shape_' + data_type + '.json', 'w') as output_json_file:
                json.dump(self.coco_output, output_json_file)
            print("Finish createCOCOJson for " + data_type + " dataset!")


if __name__ == "__main__":
    dataset_root_dir = '/home/chli/3D_FRONT/output_mask_1920x1080/'
    classes = ['accessory', 'appliance', 'art', 'basin', 'bath',
               'bed', 'build element', 'cabinet', 'chair', 'electronics',
               'kitchen cabinet', 'lighting', 'media unit', 'mirror', 'outdoor furniture',
               'plants', 'recreation', 'shelf', 'sofa', 'stair',
               'storage unit', 'table', 'wardrobe']
    data_type_list = ['train', 'test', 'val']
    thread_num = cpu_count()

    shapes_to_coco = ShapesToCOCO()

    shapes_to_coco.setDatasetRootDir(dataset_root_dir)
    shapes_to_coco.setClasses(classes)
    shapes_to_coco.setDataTypes(data_type_list)

    shapes_to_coco.createCOCOJson(thread_num)

