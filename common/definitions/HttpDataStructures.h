#pragma once

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>

struct Line {
    QString deployDate;
    QString lineCode;
    QString lineName;

    bool isValid() const {
        return !lineCode.isEmpty() && !lineName.isEmpty();
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["deployDate"] = deployDate;
        obj["lineCode"] = lineCode;
        obj["lineName"] = lineName;
        return obj;
    }

    static Line fromJson(const QJsonObject& obj) {
        Line line;
        line.deployDate = obj["deployDate"].toString();
        line.lineCode = obj["lineCode"].toString();
        line.lineName = obj["lineName"].toString();
        return line;
    }
};

struct EquipmentLine {
    QString deviceId;
    QList<Line> lines;

    bool isValid() const {
        if (lines.isEmpty()) return false;
        for (const auto& line : lines) {
            if (!line.isValid()) return false;
        }
        return !deviceId.isEmpty();
    }

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

struct CourseLine {
    QString lineCode;
    QString lineName;

    bool isValid() const {
        return !lineCode.isEmpty() && !lineName.isEmpty();
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["lineCode"] = lineCode;
        obj["lineName"] = lineName;
        return obj;
    }

    static CourseLine fromJson(const QJsonObject& obj) {
        CourseLine courseLine;
        courseLine.lineCode = obj["lineCode"].toString();
        courseLine.lineName = obj["lineName"].toString();
        return courseLine;
    }
};

struct CourseVehicle {
    QString vehicleType;
    QString applicableModel;
    QString deviceLevel;
    QString signalMode;
    QList<CourseLine> lines;

    bool isValid() const {
        if (lines.isEmpty()) return false;
        for (const auto& line : lines) {
            if (!line.isValid()) return false;
        }
        return !vehicleType.isEmpty() && !applicableModel.isEmpty();
    }

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

struct CourseAttribute {
    QString fitType;
    QString workItemTags;
    QString workItemTagsName;
    QString abilityTags;
    QString abilityTagsName;
    CourseVehicle vehicle;

    bool isValid() const {
        return !abilityTags.isEmpty() && !abilityTagsName.isEmpty() && vehicle.isValid();
    }

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

struct MKCourseInfo {
    QString courseGuid;
    QString courseName;
    QString courseDesc;
    QString courseContent;
    QString categoryId;
    QString creatorId;
    QString createTime;
    QString modifierId;
    QString modifyTime;
    QString modifyMode;
    CourseAttribute attribute;

    bool isValid() const {
        return !courseGuid.isEmpty() && !categoryId.isEmpty() &&
               !modifierId.isEmpty() && !creatorId.isEmpty() && attribute.isValid();
    }

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

struct CourseInfo {
    QString courseGuid;
    QString modifierId;
    QString modifyTime;

    bool isValid() const {
        return !courseGuid.isEmpty();
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["courseGuid"] = courseGuid;
        obj["modifierId"] = modifierId;
        obj["modifyTime"] = modifyTime;
        return obj;
    }

    static CourseInfo fromJson(const QJsonObject& obj) {
        CourseInfo info;
        info.courseGuid = obj["courseGuid"].toString();
        info.modifierId = obj["modifierId"].toString();
        info.modifyTime = obj["modifyTime"].toString();
        return info;
    }
};

struct OverhaulItem {
    QString overhaulId;
    QString overhaulName;
    QString overhaulDesc;

    bool isValid() const {
        return !overhaulId.isEmpty();
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["overhaulId"] = overhaulId;
        obj["overhaulName"] = overhaulName;
        obj["overhaulDesc"] = overhaulDesc;
        return obj;
    }

    static OverhaulItem fromJson(const QJsonObject& obj) {
        OverhaulItem item;
        item.overhaulId = obj["overhaulId"].toString();
        item.overhaulName = obj["overhaulName"].toString();
        item.overhaulDesc = obj["overhaulDesc"].toString();
        return item;
    }
};
