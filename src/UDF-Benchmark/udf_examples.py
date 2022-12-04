#!/usr/bin/python3
# -*- coding: UTF-8 -*-

from odps.udf import annotate

@annotate('string -> bigint')
class StringToBigint(object):
    def evaluate(self, arg0):
        try:
            return int(arg0)
        except ValueError:
            return 0
        except TypeError:
            return 0



@annotate("string -> double")
class StringToDouble(object):
    def evaluate(self, arg0):
        try:
            return float(arg0)
        except ValueError:
            return 0
        except TypeError:
            return 0



@annotate("string,string -> string")
class BuildUserId(object):
    def evaluate(self, user_id, imei_id):

        if user_id != None and user_id != "0":
            return user_id
        else:
            if imei_id != None and imei_id != "":
                return str(hash(imei_id))
            else:
                return "0"



@annotate("string -> array<string>")
class Spliter(object):

    """
    split string according to '\x01'
    """

    def execute(self, seg_data, length):
        try:
            data_list = seg_data.split("\x01")
            seg_num = len(data_list)

            if seg_num > length:
                data_list = data_list[0: length]
            elif seg_num < length:
                tmp_list = [None for col in range(length - seg_num)]
                data_list.extend(tmp_list)

            for index in range(len(data_list)):
                if data_list[index] == "\\N":
                    data_list[index] = None
            return data_list
                
        except Exception:
            print (Exception)
            return

@annotate("string,string,string,string,string,bigint -> double")
class NormalizeWithBase(object):
    def evaluate(self, lvbound,rvbound,value,lsbound,rsbound,desc):
        lvbound = float(lvbound)
        rvbound = float(rvbound)
        lsbound = float(lsbound)
        rsbound = float(rsbound)
        value = float(value)
        position = (rsbound-lsbound) * (value - lvbound)/(rvbound - lvbound)
        if desc:
            return rsbound - position
        else:
            return position + lsbound



@annotate("array<double>,array<double> -> double")
class PointPolygenDistance(object):
    def evaluate(self, point1, point2):
        distance = 0
        
        if len(point1)==0 or len(point2)==0 or None in point1 or None in point2:
            return distance
        elif len(point1) != len(point2):
            return distance
        else:
            for i in range(len(point1)):
                distance += (point2[i] - point1[i])**2
        return distance
