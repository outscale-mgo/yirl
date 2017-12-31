--
--Copyright (C) 2015 Matthias Gatto
--
--This program is free software: you can redistribute it and/or modify
--it under the terms of the GNU Lesser General Public License as published by
--the Free Software Foundation, either version 3 of the License, or
--(at your option) any later version.
--
--This program is distributed in the hope that it will be useful,
--but WITHOUT ANY WARRANTY; without even the implied warranty of
--MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--GNU General Public License for more details.
--
--You should have received a copy of the GNU Lesser General Public License
--along with this program.  If not, see <http://www.gnu.org/licenses/>.
--

function createArray()
   return yeCreateArray();
end

function createInt(val)
   return yeCreateInt(yLovePtrToNumber(val))
end

function createString(val)
   local ret = yeCreateString(ylovePtrToString(val))
   return ret
end

function complexFunction()
   local father = yeCreateArray()
   local e = Entity.new_int(10)

   print(e, yeGetInt(e:cent()))
   e = Entity.new_int(15, father)
   print(e, yeGetInt(e:cent()))
   e = Entity.new_string("oh you touch my tralala", father)
   print(e, yeGetString(e:cent()))
   local a = Entity.new_array()
   local a1 = Entity.new_array(a, "idx")
   e = Entity.new_string("oh my ding ding dong", a1, "str")
   print(e, a, a1, yeGetString(yeGet(yeGet(a:cent(), "idx"), "str")))
   print(e)
   print(a1.str)
   print(a[0], a[0].str)
   return 0
end
