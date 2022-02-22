empty[1] = "первый儒家"        -- Добавление элемента с целым индексом

---@diagnostic disable: lowercase-global
---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by miserable.
--- DateTime: 24.01.2022 18:58

-- Таблица общего вида:
empty = {} -- Пустая таблица
empty[1] = "первый", "xxxa"        -- Добавление элемента с целым индексом
empty[3] = "втоsdsрой"        -- Добавление элемента с целым индексом
empty["третий"] = "третий" -- Добавление элемента со строковым индексом
empty[1] = nil             -- Удаление элемента из таблицы

-- Классический массив - строки индексируются по умолчанию целыми числами, начиная с 1
days1 = {"понедельник", "вторник", "среда", "четверг", "пятница", "суббота", "воскресенье"}

-- Массив с произвольной индексацией
days2 = {[0]="воскресенье", [1]="понедельник", [2]="вторник", [3]="среда", [4]="четверг", [5]="пятница", [6]="суббота"}

-- Запись (структура) - значения различных типов индексируются литералами
person = {tabnum = 123342,                   -- Табельный номер
          fio = "Иванов Степан Васильевич",  -- Ф.И.О.
          post = "слесарь-инструментальщик", -- Должность
          salary = 25800.45,                 -- Оклад
          sdate = "23.10.2013",              -- Дата приёма на работу
          bdate = "08.08.1973"}              -- Дата рождения

pfio = person.fio --Обращение к элементу структуры.

-- Множество - индексы используются для хранения значений
workDays = {["понедельник"]=true, ["вторник"]=true, ["среда"]=true, ["четверг"]=true, ["пятница"]=true}
workDays["суббота"] = true -- Добавление субботы в число рабочих дней
workDays["среда"] = nil    -- По средам больше не работаем

for i = 1, 10 do
    print(13 .. 32)
end

d = 32

-- Проверка, является ли d рабочим днём
if workDays[d] then
    print (d.." - рабочий день")
else
    print (d.." - выходной день")
end