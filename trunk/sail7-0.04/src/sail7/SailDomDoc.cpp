/****************************************************************************

		 SailDomDoc Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/




#include <QtDebug>
#include "SailDomDoc.h"
#include "../objects/SailcutSail.h"
#include "../globals.h"


SailDomDoc::SailDomDoc(void *pSail)
{
	m_pSail = pSail;
}



void SailDomDoc::Export(QFile *pFile)
{
	SailcutSail *pSCSail  = (SailcutSail*)m_pSail;
	QDomElement data;

	QDomElement saildoc = createElement("CSaildoc");
	{
		QDomElement saildef = createElement("CSailDef");
		saildef.setAttribute("name", "saildef");
		saildoc.appendChild(saildef);

		data = createElement("enumSailCut");//for sailcut use only
		data.setAttribute("name", "sailCut");
		data.setAttribute("value", "CROSS");
		saildef.appendChild(data);

		data = createElement("enumSailType");//for sailcut use only
		data.setAttribute("name", "sailType");
		data.setAttribute("value", "MAINSAIL");
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "LOA");   //the boat length (unused)
		data.setAttribute("value", 7000);
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "foreI");   //fore triangle hoist (unused)
		data.setAttribute("value", 8940);
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "foreJ");   //fore triangle base (unused)
		data.setAttribute("value", 2850);
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "tackX");   //Leading edge position.x;
		data.setAttribute("value", (int)(pSCSail->m_LEPosition.x*1000.0));
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "tackY");   //Leading edge position.z;
		data.setAttribute("value", (int)(pSCSail->m_LEPosition.z*1000.0));
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "luffL");
		data.setAttribute("value", (int)(pSCSail->LuffLength() *1000.0));
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "rake");
		data.setAttribute("value", (int)(pSCSail->LuffLength() * tan(pSCSail->m_LuffAngle*PI/180.0) *1000.0));
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "gaffDeg");
		data.setAttribute("value", (int)(90.0 + pSCSail->m_oaSection.last()->m_Angle) - pSCSail->m_LuffAngle);
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "gaffL");
		data.setAttribute("value", (int)(pSCSail->m_oaSection.last()->m_Chord*1000.0));
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "footL");
		data.setAttribute("value", (int)(pSCSail->m_oaSection.first()->m_Chord*1000.0));
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "leechL");
		data.setAttribute("value", (int)(pSCSail->LeechLength()*1000.0));
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "luffR");
		data.setAttribute("value", 0.0);
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "gaffR");
		data.setAttribute("value", 0.0);
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "leechR");
		data.setAttribute("value", (int)(pSCSail->m_LeechRound*1000.0));
		saildef.appendChild(data);

		data = createElement("real");
		data.setAttribute("name", "footR");
		data.setAttribute("value", 0.0);
		saildef.appendChild(data);

		data = createElement("int");
		data.setAttribute("name", "luffRP");
		data.setAttribute("value", 50);
		saildef.appendChild(data);

		data = createElement("int");
		data.setAttribute("name", "gaffRP");
		data.setAttribute("value", 50);
		saildef.appendChild(data);

		data = createElement("int");
		data.setAttribute("name", "leechRP");
		data.setAttribute("value", (int)(pSCSail->m_LeechRoundPos*100.0));
		saildef.appendChild(data);

		data = createElement("int");
		data.setAttribute("name", "footRP");
		data.setAttribute("value", 50);
		saildef.appendChild(data);

		data = createElement("real");//for sailcut use only
		data.setAttribute("name", "clothW");
		data.setAttribute("value", 900);
		saildef.appendChild(data);

		data = createElement("real");//for sailcut use only
		data.setAttribute("name", "seamW");
		data.setAttribute("value", 13);
		saildef.appendChild(data);

		data = createElement("real");//for sailcut use only
		data.setAttribute("name", "leechHemW");
		data.setAttribute("value", 40);
		saildef.appendChild(data);

		data = createElement("real");//for sailcut use only
		data.setAttribute("name", "hemsW");
		data.setAttribute("value", 10);
		saildef.appendChild(data);


		data = createElement("real");//for sailcut use only
		data.setAttribute("name", "twistDeg");
		data.setAttribute("value", pSCSail->m_Twist);
		saildef.appendChild(data);


		data = createElement("real");//for sailcut use only
		data.setAttribute("name", "sheetDeg");
		data.setAttribute("value", 0);
		saildef.appendChild(data);


		QDomElement sailmould = createElement("CSailMould");
		{
			sailmould.setAttribute("name", "mould");
			saildef.appendChild(sailmould);

			data = createElement("int");
			data.setAttribute("name", "vertpos");
			data.setAttribute("value", 40);
			sailmould.appendChild(data);

			QDomElement profiledata = createElement("vector");
			profiledata.setAttribute("name", "profile");
			profiledata.setAttribute("size", pSCSail->m_oaSection.size());
			sailmould.appendChild(profiledata);

			for (int is=0; is<pSCSail->m_oaSection.size(); is++)
			{
				SailSection *pSection = pSCSail->m_oaSection.at(is);

				QDomElement profile = createElement("CProfile");
				profile.setAttribute("name", is);
				profiledata.appendChild(profile);

				double c, xc;
				pSection->m_SCSpline.GetCamber(c, xc);
				data = createElement("real");
				data.setAttribute("name", "depth");
				data.setAttribute("value", c);
				profile.appendChild(data);

				data = createElement("real");
				data.setAttribute("name", "kleech");
				data.setAttribute("value", pSection->m_SCSpline.AR);
				profile.appendChild(data);

				data = createElement("real");
				data.setAttribute("name", "kluff");
				data.setAttribute("value", pSection->m_SCSpline.AV);
				profile.appendChild(data);
			}
		}

		data = createElement("string");
		data.setAttribute("name", "sailID");
		data.setAttribute("value", pSCSail->m_SailName);
		saildef.appendChild(data);

		appendChild(saildoc);
	}

	const int IndentSize = 4;
	QTextStream out(pFile);
	save(out, IndentSize);
	pFile->close();
}


void SailDomDoc::ReadNodes(QDomNode node)
{
	while(!node.isNull())
	{
		QDomElement en = node.toElement(); // try to convert the node to an element.
		if(!en.isNull())
		{
//			qDebug()<<en.tagName()<<en.attribute("name", "No Name...") << en.attribute("value", "No value...");
		}

		// elements may also have childs...
		QDomNode m = node.firstChild();
		ReadNodes(m);
		node = node.nextSibling();
	}
}



QDomElement SailDomDoc::GetElement(QDomNode const &node, QString const &TagName, QString const &AttributeName)
{
	QDomElement en;
	QDomNode n = node;
	while(!n.isNull())
	{
		en = n.toElement(); // try to convert the node to an element.
		if(!en.isNull())
		{
			if(en.tagName()==TagName)
			{
				if(en.attribute("name","")==AttributeName) return en;
			}
		}

		// continue searching through child elements
		QDomNode m = n.firstChild();
		if(!m.isNull()) en = GetElement(m, TagName, AttributeName);
		else            en.clear();
		if(!en.isNull()) return en;

		//else continue searching
		n = n.nextSibling();
	}
	en.clear();
	return en;
}


bool SailDomDoc::Import(QFile *pFile)
{
	if(!setContent(pFile))
	{
		pFile->close();
		return false;
	}

	bool bJib=false;

	//map the minimum data we need to build the saiilcut file
	SailcutSail *pSCSail = (SailcutSail*)m_pSail;

	//create 3 sections to load
	pSCSail->m_oaSection.clear();
	for(int i=0; i<3; i++)
	{
		SailSection *pSection = new SailSection(pSCSail);
		pSCSail->m_oaSection.append(pSection);
	}

	QDomElement en;
	QDomNode n = documentElement().firstChild();

	en = GetElement(n, QString("string"), QString("sailID"));
	if(!en.isNull()) pSCSail->m_SailName = en.attribute("value", "No Name");
	else return false;

	en = GetElement(n, QString("enumSailType"), QString("sailType"));
	if(!en.isNull()) bJib = en.attribute("value", "")=="JIB";
	else return false;

	//get the Leading edge position
	en = GetElement(n, QString("real"), QString("tackX"));
	if(!en.isNull()) pSCSail->m_LEPosition.x = en.attribute("value", "0.0").toDouble()/1000.0;
	en = GetElement(n, QString("real"), QString("tackY"));
	if(!en.isNull()) pSCSail->m_LEPosition.z = en.attribute("value", "0.0").toDouble()/1000.0;
	else return false;

	//get the luff length
	double luffL = 0.0;
	en = GetElement(n, QString("real"), QString("luffL"));
	if(!en.isNull())  luffL = en.attribute("value", "0.0").toDouble()/1000.0;
	else return false;

	//get the leech length
	double leechL=0.0;
	en = GetElement(n, QString("real"), QString("leechL"));
	if(!en.isNull()) leechL = en.attribute("value", "0.0").toDouble()/1000.0;
	else return false;

	//and the rake
	double rake = 0.0;
	en = GetElement(n, QString("real"), QString("rake"));
	if(!en.isNull()) rake = en.attribute("value", "0.0").toDouble()/1000.0;
	else return false;

	//if the sail is a jib, then we need the fore triangle dimensions
	double foreI = 0.0;
	en = GetElement(n, QString("real"), QString("foreI"));
	if(!en.isNull()) foreI = en.attribute("value", "0.0").toDouble()/1000.0;
	else return false;
	double foreJ = 0.0;
	en = GetElement(n, QString("real"), QString("foreJ"));
	if(!en.isNull()) foreJ = en.attribute("value", "0.0").toDouble()/1000.0;
	else return false;

	//get the top twist
	double TopTwist = 0.0;
	en = GetElement(n, QString("real"), QString("twistDeg"));
	if(!en.isNull()) TopTwist = en.attribute("value", "0.0").toDouble();
	else return false;

	//get the foot and gaff lengths (=chords)
	en = GetElement(n, QString("real"), QString("footL"));
	if(!en.isNull()) pSCSail->m_oaSection.first()->m_Chord = en.attribute("value", "0.0").toDouble()/1000.0;
	else return false;

	en = GetElement(n, QString("real"), QString("gaffL"));
	if(!en.isNull()) pSCSail->m_oaSection.last()->m_Chord = en.attribute("value", "0.0").toDouble()/1000.0;
	else return false;

	//Get the leech round
	en = GetElement(n, QString("real"), QString("leechR"));
	if(!en.isNull()) pSCSail->m_LeechRound = en.attribute("value", "0.0").toDouble()/1000.0;
	else return false;

	en = GetElement(n, QString("int"), QString("leechRP"));
	if(!en.isNull()) pSCSail->m_LeechRoundPos = (double)en.attribute("value", "0.0").toInt()/100.0;
	else return false;

	//Get the gaff angle
	en = GetElement(n, QString("real"), QString("gaffDeg"));
	if(!en.isNull()) pSCSail->m_oaSection.last()->m_Angle = -90.0 + en.attribute("value", "0.0").toDouble();
	else return false;

	//finally load the profile data
	en = GetElement(n, QString("CSailMould"), QString("mould"));
	{
		if(en.isNull()) return false;
		// find the position of the mid-chord
		QDomElement enm = GetElement(en, QString("int"), QString("vertpos"));
		if(!enm.isNull()) pSCSail->m_oaSection[1]->SetwPosition(en.attribute("value","50").toDouble()*pSCSail->LuffLength());
		//find the size 3 vector
		enm = GetElement(en, QString("vector"), QString("profile"));
		{
			if(enm.isNull()) return false;
			int size = enm.attribute("size","3").toInt();
			// get the three profiles
			for(int ip=0; ip<size; ip++)
			{
				QDomElement enp = GetElement(enm, QString("CProfile"), QString("%1").arg(ip));
				if(enp.isNull()) return false;
				{
					QDomElement enpd = GetElement(enp, QString("real"), QString("depth"));
					if(enpd.isNull()) return false;
					double depth = enpd.attribute("value", "0.0").toDouble();
					QDomElement enpar = GetElement(enp, QString("real"), QString("kleech"));
					if(enpar.isNull()) return false;
					double ar = enpar.attribute("value", "0.0").toDouble();
					QDomElement enpav = GetElement(enp, QString("real"), QString("kluff"));
					if(enpav.isNull()) return false;
					double av = enpav.attribute("value", "0.0").toDouble();
					pSCSail->m_oaSection[ip]->SetSailcutSpline(1.0,av,ar);
					pSCSail->m_oaSection[ip]->m_SCSpline.DepthtoK(depth);
				}
			}
		}
	}


	if(bJib)
	{
		//no gaff
		pSCSail->m_oaSection.last()->m_Chord = 0.05;
		pSCSail->m_oaSection.last()->m_Angle = 0.0;
		pSCSail->m_oaSection[1]->m_Angle = (pSCSail->m_oaSection.first()->m_Angle + pSCSail->m_oaSection.last()->m_Angle)/2.0;
	}

	//convert the twist to Sail7 format
	pSCSail->m_Twist = TopTwist;


	//calculate the luff angle
	if(!bJib)
	{
		if(fabs(luffL>0)) pSCSail->SetLuffAngle(asin(rake/luffL)*180.0/PI);
		else              pSCSail->SetLuffAngle(0.0);
	}
	else
	{
		pSCSail->SetLuffAngle(atan2(foreJ,foreI)*180.0/PI);
	}
	double w = luffL * cos(pSCSail->m_LuffAngle*PI/180.0) - pSCSail->m_LEPosition.z;
	pSCSail->m_oaSection.last()->SetwPosition(w);

	//set the unused chord and u-position
	pSCSail->m_oaSection[1]->m_Position.z = (pSCSail->m_oaSection[0]->m_Position.z + pSCSail->m_oaSection[2]->m_Position.z)/2.0;
	pSCSail->m_oaSection[1]->m_Chord = (pSCSail->m_oaSection[0]->m_Chord + pSCSail->m_oaSection[2]->m_Chord)/2.0;

	// calculate the foot angle, i.e. clew position using the leech length;
	// is the intersection of two circles
	double theta0 =   0.0;
	double theta1 = 180.0;
	double c = pSCSail->FootLength();
	CVector Head, Peak, Clew;
	Head.Set(luffL*sin(pSCSail->m_LuffAngle*PI/180.0), 0.0, w);
	CVector gaff = CVector(cos(pSCSail->m_oaSection.last()->m_Angle*PI/180.0),0.0, -sin(pSCSail->m_oaSection.last()->m_Angle*PI/180.0));
	gaff *= pSCSail->m_oaSection.last()->m_Chord;
	Peak = Head + gaff;

	Clew  = CVector(c*sin((theta0+theta1)/2.0*PI/180.0), 0.0, c*cos((theta0+theta1)/2.0*PI/180.0));
	double ll = (Clew-Peak).VAbs();
	int iter=0;

	while(fabs(ll-leechL)>.01 && iter<100)
	{
		if(ll<leechL) theta0 = (theta0+theta1)/2.0;
		else          theta1 = (theta0+theta1)/2.0;
		Clew  = CVector(c*sin((theta0+theta1)/2.0*PI/180.0), 0.0, c*cos((theta0+theta1)/2.0*PI/180.0));
		ll = (Clew-Peak).VAbs();
		iter++;
	}
	pSCSail->m_oaSection.first()->m_Angle = (theta0+theta1)/2.0-90.0;

	pSCSail->SetLeechRoundEquation();
	pSCSail->SplineSurface();
	return true;
}



















