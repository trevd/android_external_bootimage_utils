/*
 * Copyright (C) 2015 Trevor Drake
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * file : lib/include/private/factory_images.h
 *
 */

#ifndef _91b5499e_92c6_11e4_80bf_5404a601fa9d
#define _91b5499e_92c6_11e4_80bf_5404a601fa9d

#define FACTORY_IMAGE_COUNT 101
#define FACTORY_IMAGE_STRNCMP_MATCH_FOUND 0
#define FACTORY_IMAGE_MAGIC_GZIP "\x1F\x8B\x08"
#define FACTORY_IMAGE_MAGIC_GZIP_SIZE 3



static struct factory_images  {
		char* name ;
		int name_length ;
		char* zip_name ;
		int zip_name_length ;
}  factory_image_info[FACTORY_IMAGE_COUNT] = {
{"fugu-lrx21m-factory-aeed3bef.tgz",32,"fugu-lrx21m/image-fugu-lrx21m.zip",33},
{"fugu-lrx21v-factory-64050f47.tgz",32,"fugu-lrx21v/image-fugu-lrx21v.zip",33},
{"hammerhead-kot49h-factory-02006b99.tgz",38,"hammerhead-kot49h/image-hammerhead-kot49h.zip",45},
{"hammerhead-krt16m-factory-bd9c39de.tgz",38,"hammerhead-krt16m/image-hammerhead-krt16m.zip",45},
{"hammerhead-ktu84m-factory-53ff95bf.tgz",38,"hammerhead-ktu84m/image-hammerhead-ktu84m.zip",45},
{"hammerhead-ktu84p-factory-35ea0277.tgz",38,"hammerhead-ktu84p/image-hammerhead-ktu84p.zip",45},
{"hammerhead-ktu84q-factory-ae475293.tgz",38,"hammerhead-ktu84q/image-hammerhead-ktu84q.zip",45},
{"hammerhead-lpv79-preview-ac1d8a8e.tgz",37,"hammerhead-lpv79/image-hammerhead-lpv79.zip",43},
{"hammerhead-lrx21o-factory-01315e08.tgz",38,"hammerhead-lrx21o/image-hammerhead-lrx21o.zip",45},
{"hammerhead-lrx22c-factory-0f9eda1b.tgz",38,"hammerhead-lrx22c/image-hammerhead-lrx22c.zip",45},
{"mantaray-jdq39-factory-d79f489e.tgz",35,"mantaray-jdq39/image-mantaray-jdq39.zip",39},
{"mantaray-jwr66y-factory-3d8252dd.tgz",36,"mantaray-jwr66y/image-mantaray-jwr66y.zip",41},
{"mantaray-kot49h-factory-174ba74f.tgz",36,"mantaray-kot49h/image-mantaray-kot49h.zip",41},
{"mantaray-krt16o-factory-85349288.tgz",36,"mantaray-krt16o/image-mantaray-krt16o.zip",41},
{"mantaray-krt16s-factory-94413961.tgz",36,"mantaray-krt16s/image-mantaray-krt16s.zip",41},
{"mantaray-ktu84l-factory-8fefbb27.tgz",36,"mantaray-ktu84l/image-mantaray-ktu84l.zip",41},
{"mantaray-ktu84p-factory-74e52998.tgz",36,"mantaray-ktu84p/image-mantaray-ktu84p.zip",41},
{"mantaray-lrx21p-factory-ad2499ea.tgz",36,"mantaray-lrx21p/image-mantaray-lrx21p.zip",41},
{"mantaray-lrx22c-factory-1a7ae5d1.tgz",36,"mantaray-lrx22c/image-mantaray-lrx22c.zip",41},
{"mysid-imm76k-factory-98d21321.tgz",33,"mysid-imm76k/image-mysid-imm76k.zip",35},
{"mysid-jdq39-factory-e365033f.tgz",32,"mysid-jdq39/image-mysid-jdq39.zip",33},
{"mysid-jro03o-factory-f17426e6.tgz",33,"mysid-jro03o/image-mysid-jro03o.zip",35},
{"mysidspr-fh05-factory-8cb5208b.tgz",34,"mysidspr-fh05/image-mysidspr-fh05.zip",37},
{"mysidspr-ga02-factory.tgz",25,"mysidspr-ga02/signed-toroplus-img-ga02.zip",42},
{"nakasig-jdq39-factory-0798439d.tgz",34,"nakasig-jdq39/image-nakasig-jdq39.zip",37},
{"nakasig-jop40c-factory-a0431f67.tgz",35,"nakasig-jop40c/image-nakasig-jop40c.zip",39},
{"nakasig-jwr66y-factory-bdbb7bd7.tgz",35,"nakasig-jwr66y/image-nakasig-jwr66y.zip",39},
{"nakasig-kot49h-factory-83d93b5f.tgz",35,"nakasig-kot49h/image-nakasig-kot49h.zip",39},
{"nakasig-krt16o-factory-9c987833.tgz",35,"nakasig-krt16o/image-nakasig-krt16o.zip",39},
{"nakasig-krt16s-factory-1e882585.tgz",35,"nakasig-krt16s/image-nakasig-krt16s.zip",39},
{"nakasig-ktu84l-factory-8ce3d5ea.tgz",35,"nakasig-ktu84l/image-nakasig-ktu84l.zip",39},
{"nakasig-ktu84p-factory-0cc2750b.tgz",35,"nakasig-ktu84p/image-nakasig-ktu84p.zip",39},
{"nakasi-jdq39-factory-c317339e.tgz",33,"nakasi-jdq39/image-nakasi-jdq39.zip",35},
{"nakasi-jop40c-factory-6aabb391.tgz",34,"nakasi-jop40c/image-nakasi-jop40c.zip",37},
{"nakasi-jro03d-factory-e102ba72.tgz",34,"nakasi-jro03d/image-nakasi-jro03d.zip",37},
{"nakasi-jwr66v-factory-d341d356.tgz",34,"nakasi-jwr66v/image-nakasi-jwr66v.zip",37},
{"nakasi-jwr66y-factory-1e503532.tgz",34,"nakasi-jwr66y/image-nakasi-jwr66y.zip",37},
{"nakasi-jzo54k-factory-973f190e.tgz",34,"nakasi-jzo54k/image-nakasi-jzo54k.zip",37},
{"nakasi-kot49h-factory-5e9db5e1.tgz",34,"nakasi-kot49h/image-nakasi-kot49h.zip",37},
{"nakasi-krt16o-factory-db4a1a8a.tgz",34,"nakasi-krt16o/image-nakasi-krt16o.zip",37},
{"nakasi-krt16s-factory-da7dee49.tgz",34,"nakasi-krt16s/image-nakasi-krt16s.zip",37},
{"nakasi-ktu84l-factory-0e21238f.tgz",34,"nakasi-ktu84l/image-nakasi-ktu84l.zip",37},
{"nakasi-ktu84p-factory-76acdbe9.tgz",34,"nakasi-ktu84p/image-nakasi-ktu84p.zip",37},
{"nakasi-lrx21p-factory-93daa4d3.tgz",34,"nakasi-lrx21p/image-nakasi-lrx21p.zip",37},
{"nakasi-lrx22g-factory-2291c36b.tgz",34,"nakasi-lrx22g/image-nakasi-lrx22g.zip",37},
{"occam-jdq39-factory-345dc199.tgz",32,"occam-jdq39/image-occam-jdq39.zip",33},
{"occam-jwr66y-factory-74b1deab.tgz",33,"occam-jwr66y/image-occam-jwr66y.zip",35},
{"occam-kot49h-factory-02e344de.tgz",33,"occam-kot49h/image-occam-kot49h.zip",35},
{"occam-krt16o-factory-75ccae7a.tgz",33,"occam-krt16o/image-occam-krt16o.zip",35},
{"occam-krt16s-factory-2006f418.tgz",33,"occam-krt16s/image-occam-krt16s.zip",35},
{"occam-ktu84l-factory-0d3fd624.tgz",33,"occam-ktu84l/image-occam-ktu84l.zip",35},
{"occam-ktu84p-factory-b6ac3ad6.tgz",33,"occam-ktu84p/image-occam-ktu84p.zip",35},
{"occam-lrx21t-factory-51cee750.tgz",33,"occam-lrx21t/image-occam-lrx21t.zip",35},
{"occam-lrx22c-factory-86c04af6.tgz",33,"occam-lrx22c/image-occam-lrx22c.zip",35},
{"razorg-JLS36C-factory-fb03a89f.tgz",34,"razorg-JLS36C/image-razorg-JLS36C.zip",37},
{"razorg-jls36i-factory-ecb320cd.tgz",34,"razorg-jls36i/image-razorg-jls36i.zip",37},
{"razorg-kot49h-factory-49789b24.tgz",34,"razorg-kot49h/image-razorg-kot49h.zip",37},
{"razorg-krt16o-factory-2b749c29.tgz",34,"razorg-krt16o/image-razorg-krt16o.zip",37},
{"razorg-krt16s-factory-bd6c9241.tgz",34,"razorg-krt16s/image-razorg-krt16s.zip",37},
{"razorg-ktu84l-factory-9f9b9ef2.tgz",34,"razorg-ktu84l/image-razorg-ktu84l.zip",37},
{"razorg-ktu84p-factory-f21762aa.tgz",34,"razorg-ktu84p/image-razorg-ktu84p.zip",37},
{"razorg-kvt49l-factory-65bdbe0a.tgz",34,"razorg-kvt49l/image-razorg-kvt49l.zip",37},
{"razor-jss15q-factory-4f77b811.tgz",33,"razor-jss15q/image-razor-jss15q.zip",35},
{"razor-jss15r-factory-ec2d4f76.tgz",33,"razor-jss15r/image-razor-jss15r.zip",35},
{"razor-kot49h-factory-ebb4918e.tgz",33,"razor-kot49h/image-razor-kot49h.zip",35},
{"razor-krt16o-factory-d9e7d441.tgz",33,"razor-krt16o/image-razor-krt16o.zip",35},
{"razor-krt16s-factory-7235eb0d.tgz",33,"razor-krt16s/image-razor-krt16s.zip",35},
{"razor-ktu84l-factory-afe3afc8.tgz",33,"razor-ktu84l/image-razor-ktu84l.zip",35},
{"razor-ktu84p-factory-b1b2c0da.tgz",33,"razor-ktu84p/image-razor-ktu84p.zip",35},
{"razor-lpv79-preview-d0ddf8ce.tgz",32,"razor-lpv79/image-razor-lpv79.zip",33},
{"razor-lrx21p-factory-ba55c6ab.tgz",33,"razor-lrx21p/image-razor-lrx21p.zip",35},
{"razor-lrx22c-factory-a9c6e55f.tgz",33,"razor-lrx22c/image-razor-lrx22c.zip",35},
{"shamu-lrx21o-factory-e028f5ea.tgz",33,"shamu-lrx21o/image-shamu-lrx21o.zip",35},
{"shamu-lrx22c-factory-ff173fc6.tgz",33,"shamu-lrx22c/image-shamu-lrx22c.zip",35},
{"sojua-grk39f-factory-5d73a09d.tgz",33,"sojua-grk39f/image-sojua-grk39f.zip",35},
{"sojua-imm76d-factory-76ad4959.tgz",33,"sojua-imm76d/image-sojua-imm76d.zip",35},
{"sojua-jzo54k-factory-1121b619.tgz",33,"sojua-jzo54k/image-sojua-jzo54k.zip",35},
{"soju-grk39f-factory-5ab09c98.tgz",32,"soju-grk39f/image-soju-grk39f.zip",33},
{"soju-imm76d-factory-ca4ae9ee.tgz",32,"soju-imm76d/image-soju-imm76d.zip",33},
{"soju-jzo54k-factory-36602333.tgz",32,"soju-jzo54k/image-soju-jzo54k.zip",33},
{"sojuk-grk39f-factory-4d5663c0.tgz",33,"sojuk-grk39f/image-sojuk-grk39f.zip",35},
{"sojuk-imm76d-factory-422adc36.tgz",33,"sojuk-imm76d/image-sojuk-imm76d.zip",35},
{"sojuk-jro03e-factory-93a21b70.tgz",33,"sojuk-jro03e/image-sojuk-jro03e.zip",35},
{"sojus-gwk74-factory-4a34b67a.tgz",32,"sojus-gwk74/image-sojus-gwk74.zip",33},
{"sojus-imm76d-factory-10660f4c.tgz",33,"sojus-imm76d/image-sojus-imm76d.zip",35},
{"sojus-jro03r-factory-59a247f5.tgz",33,"sojus-jro03r/image-sojus-jro03r.zip",35},
{"takju-imm76i-factory-e8c33767.tgz",33,"takju-imm76i/image-takju-imm76i.zip",35},
{"takju-jdq39-factory-5e273f02.tgz",32,"takju-jdq39/image-takju-jdq39.zip",33},
{"takju-jwr66y-factory-5104ab1d.tgz",33,"takju-jwr66y/image-takju-jwr66y.zip",35},
{"takju-jzo54k-factory-92830c0b.tgz",33,"takju-jzo54k/image-takju-jzo54k.zip",35},
{"tungsten-ian67k-factory-468d9865.tgz",36,"tungsten-ian67k/image-tungsten-ian67k.zip",41},
{"volantisg-lrx22c-factory-a9668749.tgz",37,"volantisg-lrx22c/image-volantisg-lrx22c.zip",43},
{"volantis-lrx21l-factory-dc7e3fc7.tgz",36,"volantis-lrx21l/image-volantis-lrx21l.zip",41},
{"volantis-lrx21q-factory-10521789.tgz",36,"volantis-lrx21q/image-volantis-lrx21q.zip",41},
{"volantis-lrx21r-factory-ac87eba2.tgz",36,"volantis-lrx21r/image-volantis-lrx21r.zip",41},
{"volantis-lrx22c-factory-8d83cd9b.tgz",36,"volantis-lrx22c/image-volantis-lrx22c.zip",41},
{"yakju-imm76i-factory-8001e72f.tgz",33,"yakju-imm76i/image-yakju-imm76i.zip",35},
{"yakju-jdq39-factory-b2ebb5f3.tgz",32,"yakju-jdq39/image-yakju-jdq39.zip",33},
{"yakju-jwr66y-factory-09207065.tgz",33,"yakju-jwr66y/image-yakju-jwr66y.zip",35},
{"yakju-jzo54k-factory-92ff9457.tgz",33,"yakju-jzo54k/image-yakju-jzo54k.zip",35},
{ NULL,-1, NULL, -1 },
} ;
#endif
