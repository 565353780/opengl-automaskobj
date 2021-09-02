#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import json

import cv2
import matplotlib
import matplotlib.pyplot as plt

from pycocotools.coco import COCO

class COCODatasetLoader:
    def __init__(self):
        self.reset()
        return

    def reset(self):
        self.dataset_root_dir = None
        self.image_dir = None
        self.annotation_dir = None
        self.valid_dataset_type_list = ['train', 'val']
        self.data_type_list = []
        # [[data_type, coco_json, coco_json_file_path], ...]
        self.coco_json_list = []
        self.classes = []
        # [[data_type, [[image_id, image_file_path, image_size], ...]], ...]
        self.image_info_list = []
        # [[data_type, [[image_id, image_file_path, image_size, annotation_id], ...]], ...]
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
        self.reset()

        self.dataset_root_dir = dataset_root_dir
        if self.dataset_root_dir[-1] != "/":
            self.dataset_root_dir += "/"
        return

    def getDataTypes(self):
        self.data_type_list = []

        dataset_root_dir_list = os.listdir(self.dataset_root_dir)
        for dataset_root_dir in dataset_root_dir_list:
            if dataset_root_dir in self.valid_dataset_type_list:
                if dataset_root_dir not in self.data_type_list:
                    self.data_type_list.append(dataset_root_dir)
        return

    def loadCOCOJson(self):
        self.coco_json_file_path_list = []
        self.coco_json_list = []

        for data_type in self.data_type_list:
            coco_json_file_folder_path = \
                self.dataset_root_dir + data_type + "/"

            coco_json_file_folder_filename_list = os.listdir(coco_json_file_folder_path)

            coco_json_file_path = None
            for filename in coco_json_file_folder_filename_list:
                if filename[-5:] == ".json":
                    coco_json_file_path = coco_json_file_folder_path + filename
                    break

            with open(coco_json_file_path, "r") as f:
                coco_json = json.load(f)
                self.coco_json_list.append([data_type, coco_json, coco_json_file_path])
        return

    def loadClasses(self):
        self.classes = []
        return

    def loadImageInfoList(self):
        self.image_info_list = []
        for data_type in self.data_type_list:
            image_info_list = []
            image_dir = self.dataset_root_dir + data_type + "/images/"

            for coco_json_pair in self.coco_json_list:
                if coco_json_pair[0] != data_type:
                    continue

                coco_json = coco_json_pair[1]
                for image_info_json in coco_json["images"]:
                    image_id = int(image_info_json["id"])
                    image_file_name = image_info_json["file_name"]
                    image_file_path = image_dir + image_file_name
                    image_width = image_info_json["width"]
                    image_height = image_info_json["height"]
                    image_info_list.append([image_id, image_file_path, [image_width, image_height]])

            self.image_info_list.append([data_type, image_info_list])
        return

    def loadAnnotationInfoList(self):
        self.annotation_info_list = []
        for data_type in self.data_type_list:
            annotation_info_list = []
            annotation_dir = self.dataset_root_dir + data_type + "/annotations/"

            for coco_json_pair in self.coco_json_list:
                if coco_json_pair[0] != data_type:
                    continue

                coco_json = coco_json_pair[1]
                for annotation_info_json in coco_json["annotations"]:
                    image_id = int(annotation_info_json["image_id"])
                    image_file_path = None
                    image_size = [annotation_info_json["width"], annotation_info_json["height"]]
                    for image_info_pair in self.image_info_list:
                        if image_info_pair[0] != data_type:
                            continue

                        image_info_list = image_info_pair[1]
                        for image_info in image_info_list:
                            if image_info[0] == image_id:
                                image_file_path = image_info[1]
                                image_size = image_info[2]
                                break
                        if image_file_path is not None:
                            break
                    annotation_id = annotation_info_json["id"]
                    annotation_info_list.append([image_id, image_file_path, image_size, annotation_id])

            self.annotation_info_list.append([data_type, annotation_info_list])
        return

    def loadCOCODataset(self, dataset_root_dir):
        self.setDatasetRootDir(dataset_root_dir)
        self.getDataTypes()
        self.loadCOCOJson()
        self.loadClasses()
        self.loadImageInfoList()
        self.loadAnnotationInfoList()
        return

    def getImageInfoList(self, data_type):
        for image_info_pair in self.image_info_list:
            if image_info_pair[0] != data_type:
                continue
            return image_info_pair[1]
        print("COCODatasetLoader::getImageInfoList : data_type : " + data_type + " not found!")
        return None

    def getAnnotationInfoList(self, data_type):
        for annotation_info_pair in self.annotation_info_list:
            if annotation_info_pair[0] != data_type:
                continue
            return annotation_info_pair[1]
        print("COCODatasetLoader::getAnnotationInfoList : data_type : " + data_type + " not found!")
        return None

    def getCOCOJsonData(self, data_type):
        for coco_json_pair in self.coco_json_list:
            if coco_json_pair[0] != data_type:
                continue
            return coco_json_pair[1]
        print("COCODatasetLoader::getCOCOJsonFIlePath : data_type : " + data_type + " not found!")
        return None

    def getCOCOJsonFIlePath(self, data_type):
        for coco_json_pair in self.coco_json_list:
            if coco_json_pair[0] != data_type:
                continue
            return coco_json_pair[2]
        print("COCODatasetLoader::getCOCOJsonFIlePath : data_type : " + data_type + " not found!")
        return None

    def visualCOCODataset(self, vis_data_type):
        annotation_info_list = coco_dataset_loader.getAnnotationInfoList(vis_data_type)
        coco_json_file_path = coco_dataset_loader.getCOCOJsonFIlePath(vis_data_type)

        cc = COCO(coco_json_file_path)

        for annotation_info in annotation_info_list:
            image_id = annotation_info[0]
            image_file_path = annotation_info[1]
            image_size = annotation_info[2]
            annotation_id = annotation_info[3]

            im = cv2.imread(image_file_path)
            plt.imshow(im)
            plt.axis('off')

            annIds = cc.getAnnIds(imgIds=image_id)
            anns = cc.loadAnns(annIds)
            cc.showAnns(anns)
            plt.show()

        return


if __name__ == '__main__':
    dataset_root_dir = "/home/chli/3D_FRONT/output_mask_test/"
    vis_data_type = 'train'

    coco_dataset_loader = COCODatasetLoader()

    coco_dataset_loader.loadCOCODataset(dataset_root_dir)

    coco_dataset_loader.visualCOCODataset(vis_data_type)

    
