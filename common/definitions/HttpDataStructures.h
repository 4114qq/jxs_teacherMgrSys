/**
 * @file HttpDataStructures.h
 * @brief HTTP数据结构定义
 * @details 定义课程、设备线路等数据传输对象
 */

#pragma once

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>

/**
 * @struct Line
 * @brief 线路结构
 * @details 表示一条线路的基本信息
 */
struct Line {
    QString deployDate;   ///< 部署日期
    QString lineCode;    ///< 线路代码
    QString lineName;    ///< 线路名称

    /**
     * @brief 检查数据有效性
     * @return bool 是否有效
     */
    bool isValid() const {
        return !lineCode.isEmpty() && !lineName.isEmpty();
    }

    /**
     * @brief 转换为JSON对象
     * @return QJsonObject JSON对象
     */
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["deployDate"] = deployDate;
        obj["lineCode"] = lineCode;
        obj["lineName"] = lineName;
        return obj;
    }

    /**
     * @brief 从JSON对象构造
     * @param obj JSON对象
     * @return Line 线路结构
     */
    static Line fromJson(const QJsonObject& obj) {
        Line line;
        line.deployDate = obj["deployDate"].toString();
        line.lineCode = obj["lineCode"].toString();
        line.lineName = obj["lineName"].toString();
        return line;
    }
};

/**
 * @struct EquipmentLine
 * @brief 设备线路结构
 * @details 表示设备及其包含的线路信息
 */
struct EquipmentLine {
    QString deviceId;     ///< 设备ID
    QList<Line> lines;   ///< 线路列表

    /**
     * @brief 检查数据有效性
     * @return bool 是否有效
     */
    bool isValid() const {
        if (lines.isEmpty()) return false;
        for (const auto& line : lines) {
            if (!line.isValid()) return false;
        }
        return !deviceId.isEmpty();
    }

    /**
     * @brief 转换为JSON对象
     * @return QJsonObject JSON对象
     */
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["deviceId"] = deviceId;
        QJsonArray linesArray;
        for (const auto& line : lines) {
            linesArray.append(line.toJson());
        }
        obj["lines"] = linesArray;
        return obj;
    }

    /**
     * @brief 从JSON对象构造
     * @param obj JSON对象
     * @return EquipmentLine 设备线路结构
     */
    static EquipmentLine fromJson(const QJsonObject& obj) {
        EquipmentLine eqLine;
        eqLine.deviceId = obj["deviceId"].toString();
        QJsonArray linesArray = obj["lines"].toArray();
        for (const auto& item : linesArray) {
            eqLine.lines.append(Line::fromJson(item.toObject()));
        }
        return eqLine;
    }
};

/**
 * @struct CourseLine
 * @brief 课程线路结构
 * @details 表示课程中的线路信息
 */
struct CourseLine {
    QString lineCode;    ///< 线路代码
    QString lineName;    ///< 线路名称

    /**
     * @brief 检查数据有效性
     * @return bool 是否有效
     */
    bool isValid() const {
        return !lineCode.isEmpty() && !lineName.isEmpty();
    }

    /**
     * @brief 转换为JSON对象
     * @return QJsonObject JSON对象
     */
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["lineCode"] = lineCode;
        obj["lineName"] = lineName;
        return obj;
    }

    /**
     * @brief 从JSON对象构造
     * @param obj JSON对象
     * @return CourseLine 课程线路结构
     */
    static CourseLine fromJson(const QJsonObject& obj) {
        CourseLine courseLine;
        courseLine.lineCode = obj["lineCode"].toString();
        courseLine.lineName = obj["lineName"].toString();
        return courseLine;
    }
};

/**
 * @struct CourseVehicle
 * @brief 课程车型结构
 * @details 表示课程中的车型及相关线路信息
 */
struct CourseVehicle {
    QString vehicleType;      ///< 车型
    QString applicableModel;  ///< 适用型号
    QString deviceLevel;      ///< 设备等级
    QString signalMode;      ///< 信号模式
    QList<CourseLine> lines; ///< 线路列表

    /**
     * @brief 检查数据有效性
     * @return bool 是否有效
     */
    bool isValid() const {
        if (lines.isEmpty()) return false;
        for (const auto& line : lines) {
            if (!line.isValid()) return false;
        }
        return !vehicleType.isEmpty() && !applicableModel.isEmpty();
    }

    /**
     * @brief 转换为JSON对象
     * @return QJsonObject JSON对象
     */
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["vehicleType"] = vehicleType;
        obj["applicableModel"] = applicableModel;
        obj["deviceLevel"] = deviceLevel;
        obj["signalMode"] = signalMode;
        QJsonArray linesArray;
        for (const auto& line : lines) {
            linesArray.append(line.toJson());
        }
        obj["lines"] = linesArray;
        return obj;
    }

    /**
     * @brief 从JSON对象构造
     * @param obj JSON对象
     * @return CourseVehicle 课程车型结构
     */
    static CourseVehicle fromJson(const QJsonObject& obj) {
        CourseVehicle vehicle;
        vehicle.vehicleType = obj["vehicleType"].toString();
        vehicle.applicableModel = obj["applicableModel"].toString();
        vehicle.deviceLevel = obj["deviceLevel"].toString();
        vehicle.signalMode = obj["signalMode"].toString();
        QJsonArray linesArray = obj["lines"].toArray();
        for (const auto& item : linesArray) {
            vehicle.lines.append(CourseLine::fromJson(item.toObject()));
        }
        return vehicle;
    }
};

/**
 * @struct CourseAttribute
 * @brief 课程属性结构
 * @details 表示课程的属性信息，包括适配类型、工作项目标签、能力标签等
 */
struct CourseAttribute {
    QString fitType;         ///< 适配类型
    QString workItemTags;    ///< 工作项目标签
    QString workItemTagsName; ///< 工作项目标签名称
    QString abilityTags;     ///< 能力标签
    QString abilityTagsName; ///< 能力标签名称
    CourseVehicle vehicle;   ///< 课程车型

    /**
     * @brief 检查数据有效性
     * @return bool 是否有效
     */
    bool isValid() const {
        return !abilityTags.isEmpty() && !abilityTagsName.isEmpty() && vehicle.isValid();
    }

    /**
     * @brief 转换为JSON对象
     * @return QJsonObject JSON对象
     */
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["fitType"] = fitType;
        obj["workItemTags"] = workItemTags;
        obj["workItemTagsName"] = workItemTagsName;
        obj["abilityTags"] = abilityTags;
        obj["abilityTagsName"] = abilityTagsName;
        obj["vehicle"] = vehicle.toJson();
        return obj;
    }

    /**
     * @brief 从JSON对象构造
     * @param obj JSON对象
     * @return CourseAttribute 课程属性结构
     */
    static CourseAttribute fromJson(const QJsonObject& obj) {
        CourseAttribute attr;
        attr.fitType = obj["fitType"].toString();
        attr.workItemTags = obj["workItemTags"].toString();
        attr.workItemTagsName = obj["workItemTagsName"].toString();
        attr.abilityTags = obj["abilityTags"].toString();
        attr.abilityTagsName = obj["abilityTagsName"].toString();
        attr.vehicle = CourseVehicle::fromJson(obj["vehicle"].toObject());
        return attr;
    }
};

/**
 * @struct MKCourseInfo
 * @brief 课程信息结构
 * @details 表示课程完整信息，包括基本信息和属性
 */
struct MKCourseInfo {
    QString courseGuid;    ///< 课程GUID
    QString courseName;    ///< 课程名称
    QString courseDesc;    ///< 课程描述
    QString courseContent; ///< 课程内容
    QString categoryId;    ///< 分类ID
    QString creatorId;     ///< 创建者ID
    QString createTime;    ///< 创建时间
    QString modifierId;     ///< 修改者ID
    QString modifyTime;    ///< 修改时间
    QString modifyMode;    ///< 修改模式
    CourseAttribute attribute; ///< 课程属性

    /**
     * @brief 检查数据有效性
     * @return bool 是否有效
     */
    bool isValid() const {
        return !courseGuid.isEmpty() && !categoryId.isEmpty() &&
               !modifierId.isEmpty() && !creatorId.isEmpty() && attribute.isValid();
    }

    /**
     * @brief 转换为JSON对象
     * @return QJsonObject JSON对象
     */
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["courseGuid"] = courseGuid;
        obj["courseName"] = courseName;
        obj["courseDesc"] = courseDesc;
        obj["courseContent"] = courseContent;
        obj["categoryId"] = categoryId;
        obj["creatorId"] = creatorId;
        obj["createTime"] = createTime;
        obj["modifierId"] = modifierId;
        obj["modifyTime"] = modifyTime;
        obj["modifyMode"] = modifyMode;
        obj["attribute"] = attribute.toJson();
        return obj;
    }

    /**
     * @brief 从JSON对象构造
     * @param obj JSON对象
     * @return MKCourseInfo 课程信息结构
     */
    static MKCourseInfo fromJson(const QJsonObject& obj) {
        MKCourseInfo course;
        course.courseGuid = obj["courseGuid"].toString();
        course.courseName = obj["courseName"].toString();
        course.courseDesc = obj["courseDesc"].toString();
        course.courseContent = obj["courseContent"].toString();
        course.categoryId = obj["categoryId"].toString();
        course.creatorId = obj["creatorId"].toString();
        course.createTime = obj["createTime"].toString();
        course.modifierId = obj["modifierId"].toString();
        course.modifyTime = obj["modifyTime"].toString();
        course.modifyMode = obj["modifyMode"].toString();
        course.attribute = CourseAttribute::fromJson(obj["attribute"].toObject());
        return course;
    }
};

/**
 * @struct CourseInfo
 * @brief 课程基本信息结构
 * @details 用于课程删除等轻量级操作
 */
struct CourseInfo {
    QString courseGuid;    ///< 课程GUID
    QString modifierId;   ///< 修改者ID
    QString modifyTime;   ///< 修改时间

    /**
     * @brief 检查数据有效性
     * @return bool 是否有效
     */
    bool isValid() const {
        return !courseGuid.isEmpty();
    }

    /**
     * @brief 转换为JSON对象
     * @return QJsonObject JSON对象
     */
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["courseGuid"] = courseGuid;
        obj["modifierId"] = modifierId;
        obj["modifyTime"] = modifyTime;
        return obj;
    }

    /**
     * @brief 从JSON对象构造
     * @param obj JSON对象
     * @return CourseInfo 课程信息结构
     */
    static CourseInfo fromJson(const QJsonObject& obj) {
        CourseInfo info;
        info.courseGuid = obj["courseGuid"].toString();
        info.modifierId = obj["modifierId"].toString();
        info.modifyTime = obj["modifyTime"].toString();
        return info;
    }
};

/**
 * @struct OverhaulItem
 * @brief 检修项目结构
 * @details 表示检修项目的基本信息
 */
struct OverhaulItem {
    QString overhaulId;    ///< 检修项目ID
    QString overhaulName;  ///< 检修项目名称
    QString overhaulDesc;  ///< 检修项目描述

    /**
     * @brief 检查数据有效性
     * @return bool 是否有效
     */
    bool isValid() const {
        return !overhaulId.isEmpty();
    }

    /**
     * @brief 转换为JSON对象
     * @return QJsonObject JSON对象
     */
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["overhaulId"] = overhaulId;
        obj["overhaulName"] = overhaulName;
        obj["overhaulDesc"] = overhaulDesc;
        return obj;
    }

    /**
     * @brief 从JSON对象构造
     * @param obj JSON对象
     * @return OverhaulItem 检修项目结构
     */
    static OverhaulItem fromJson(const QJsonObject& obj) {
        OverhaulItem item;
        item.overhaulId = obj["overhaulId"].toString();
        item.overhaulName = obj["overhaulName"].toString();
        item.overhaulDesc = obj["overhaulDesc"].toString();
        return item;
    }
};
