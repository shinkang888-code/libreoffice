# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the lofice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Jar_Jar,reportbuilder))

$(eval $(call gb_Jar_use_jars,reportbuilder,\
	java_uno \
	libreoffice \
))

$(eval $(call gb_Jar_use_externals,reportbuilder,\
	flow-engine \
	flute \
	libbase \
	libfonts \
	libformula \
	liblayout \
	libloader \
	librepository \
	libserializer \
	libxml \
	sac \
))

$(eval $(call gb_Jar_set_manifest,reportbuilder,$(SRCDIR)/reportbuilder/java/manifest.mf))

$(eval $(call gb_Jar_set_componentfile,reportbuilder,reportbuilder/java/reportbuilder,OOO,services))

$(eval $(call gb_Jar_set_packageroot,reportbuilder,org))

$(eval $(call gb_Jar_add_sourcefiles,reportbuilder,\
    reportbuilder/java/org/lofice/report/DataRow \
    reportbuilder/java/org/lofice/report/DataSource \
    reportbuilder/java/org/lofice/report/DataSourceException \
    reportbuilder/java/org/lofice/report/DataSourceFactory \
    reportbuilder/java/org/lofice/report/ImageService \
    reportbuilder/java/org/lofice/report/InputRepository \
    reportbuilder/java/org/lofice/report/JobDefinitionException \
    reportbuilder/java/org/lofice/report/JobProperties \
    reportbuilder/java/org/lofice/report/OfficeToken \
    reportbuilder/java/org/lofice/report/OutputRepository \
    reportbuilder/java/org/lofice/report/ParameterMap \
    reportbuilder/java/org/lofice/report/ReportEngineMetaData \
    reportbuilder/java/org/lofice/report/ReportEngineParameterNames \
    reportbuilder/java/org/lofice/report/ReportExecutionException \
    reportbuilder/java/org/lofice/report/ReportJob \
    reportbuilder/java/org/lofice/report/ReportJobDefinition \
    reportbuilder/java/org/lofice/report/ReportJobFactory \
    reportbuilder/java/org/lofice/report/SDBCReportData \
    reportbuilder/java/org/lofice/report/SDBCReportDataFactory \
    reportbuilder/java/org/lofice/report/SOImageService \
    reportbuilder/java/org/lofice/report/StorageRepository \
    reportbuilder/java/org/lofice/report/function/metadata/AuthorFunction \
    reportbuilder/java/org/lofice/report/function/metadata/AuthorFunctionDescription \
    reportbuilder/java/org/lofice/report/function/metadata/MetaDataFunctionCategory \
    reportbuilder/java/org/lofice/report/function/metadata/TitleFunction \
    reportbuilder/java/org/lofice/report/function/metadata/TitleFunctionDescription \
    reportbuilder/java/org/lofice/report/pentaho/DefaultNameGenerator \
    reportbuilder/java/org/lofice/report/pentaho/OfficeNamespaces \
    reportbuilder/java/org/lofice/report/pentaho/PentahoFormulaContext \
    reportbuilder/java/org/lofice/report/pentaho/PentahoReportEngine \
    reportbuilder/java/org/lofice/report/pentaho/PentahoReportEngineMetaData \
    reportbuilder/java/org/lofice/report/pentaho/PentahoReportJob \
    reportbuilder/java/org/lofice/report/pentaho/SOFormulaOpCodeMapper \
    reportbuilder/java/org/lofice/report/pentaho/SOFormulaParser \
    reportbuilder/java/org/lofice/report/pentaho/SOFunctionManager \
    reportbuilder/java/org/lofice/report/pentaho/SOReportJobFactory \
    reportbuilder/java/org/lofice/report/pentaho/StarFunctionCategory \
    reportbuilder/java/org/lofice/report/pentaho/StarFunctionDescription \
    reportbuilder/java/org/lofice/report/pentaho/StarReportData \
    reportbuilder/java/org/lofice/report/pentaho/StarReportDataFactory \
    reportbuilder/java/org/lofice/report/pentaho/StarReportModule \
    reportbuilder/java/org/lofice/report/pentaho/expressions/SumExpression \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/AbstractReportElementLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/FixedTextLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/FormatValueUtility \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/FormattedTextLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/ImageElementContext \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/ImageElementLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/ObjectOleLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/OfficeDetailLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/OfficeGroupInstanceSectionLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/OfficeGroupLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/OfficeGroupSectionLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/OfficePageSectionLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/OfficeRepeatingStructureLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/OfficeReportLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/OfficeTableLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/OfficeTableTemplateLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/TableCellLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/VariablesCollection \
    reportbuilder/java/org/lofice/report/pentaho/layoutprocessor/VariablesDeclarationLayoutController \
    reportbuilder/java/org/lofice/report/pentaho/loader/InputRepositoryLoader \
    reportbuilder/java/org/lofice/report/pentaho/loader/InputRepositoryResourceData \
    reportbuilder/java/org/lofice/report/pentaho/loader/InputResourceKey \
    reportbuilder/java/org/lofice/report/pentaho/model/DataStyle \
    reportbuilder/java/org/lofice/report/pentaho/model/FixedTextElement \
    reportbuilder/java/org/lofice/report/pentaho/model/FontFaceDeclsSection \
    reportbuilder/java/org/lofice/report/pentaho/model/FontFaceElement \
    reportbuilder/java/org/lofice/report/pentaho/model/FormatCondition \
    reportbuilder/java/org/lofice/report/pentaho/model/FormattedTextElement \
    reportbuilder/java/org/lofice/report/pentaho/model/ImageElement \
    reportbuilder/java/org/lofice/report/pentaho/model/ObjectOleElement \
    reportbuilder/java/org/lofice/report/pentaho/model/OfficeDetailSection \
    reportbuilder/java/org/lofice/report/pentaho/model/OfficeDocument \
    reportbuilder/java/org/lofice/report/pentaho/model/OfficeGroup \
    reportbuilder/java/org/lofice/report/pentaho/model/OfficeGroupInstanceSection \
    reportbuilder/java/org/lofice/report/pentaho/model/OfficeGroupSection \
    reportbuilder/java/org/lofice/report/pentaho/model/OfficeMasterPage \
    reportbuilder/java/org/lofice/report/pentaho/model/OfficeMasterStyles \
    reportbuilder/java/org/lofice/report/pentaho/model/OfficeReport \
    reportbuilder/java/org/lofice/report/pentaho/model/OfficeStyle \
    reportbuilder/java/org/lofice/report/pentaho/model/OfficeStyles \
    reportbuilder/java/org/lofice/report/pentaho/model/OfficeStylesCollection \
    reportbuilder/java/org/lofice/report/pentaho/model/OfficeTableSection \
    reportbuilder/java/org/lofice/report/pentaho/model/PageLayout \
    reportbuilder/java/org/lofice/report/pentaho/model/PageSection \
    reportbuilder/java/org/lofice/report/pentaho/model/RawText \
    reportbuilder/java/org/lofice/report/pentaho/model/ReportElement \
    reportbuilder/java/org/lofice/report/pentaho/model/TableCellElement \
    reportbuilder/java/org/lofice/report/pentaho/model/VariablesDeclarationSection \
    reportbuilder/java/org/lofice/report/pentaho/output/ImageProducer \
    reportbuilder/java/org/lofice/report/pentaho/output/OfficeDocumentReportTarget \
    reportbuilder/java/org/lofice/report/pentaho/output/OleProducer \
    reportbuilder/java/org/lofice/report/pentaho/output/StyleUtilities \
    reportbuilder/java/org/lofice/report/pentaho/output/StylesWriter \
    reportbuilder/java/org/lofice/report/pentaho/output/chart/ChartRawReportProcessor \
    reportbuilder/java/org/lofice/report/pentaho/output/chart/ChartRawReportTarget \
    reportbuilder/java/org/lofice/report/pentaho/output/spreadsheet/SpreadsheetRawReportProcessor \
    reportbuilder/java/org/lofice/report/pentaho/output/spreadsheet/SpreadsheetRawReportTarget \
    reportbuilder/java/org/lofice/report/pentaho/output/text/MasterPageFactory \
    reportbuilder/java/org/lofice/report/pentaho/output/text/PageBreakDefinition \
    reportbuilder/java/org/lofice/report/pentaho/output/text/PageContext \
    reportbuilder/java/org/lofice/report/pentaho/output/text/TextRawReportProcessor \
    reportbuilder/java/org/lofice/report/pentaho/output/text/TextRawReportTarget \
    reportbuilder/java/org/lofice/report/pentaho/output/text/VariablesDeclarations \
    reportbuilder/java/org/lofice/report/pentaho/parser/ElementReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/OfficeDocumentXmlResourceFactory \
    reportbuilder/java/org/lofice/report/pentaho/parser/OfficeStylesXmlResourceFactory \
    reportbuilder/java/org/lofice/report/pentaho/parser/StarStyleXmlFactoryModule \
    reportbuilder/java/org/lofice/report/pentaho/parser/StarXmlFactoryModule \
    reportbuilder/java/org/lofice/report/pentaho/parser/chart/ChartReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/data/DataStyleReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/draw/ObjectOleReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/office/BodyReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/office/DocumentContentReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/office/DocumentStylesReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/office/FontFaceDeclsReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/office/MasterStylesReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/ConditionalPrintExpressionReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/DetailRootTableReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/FixedContentReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/FormatConditionReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/FormattedTextReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/FunctionReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/GroupReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/GroupSectionReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/ImageReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/MasterDetailReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/ReportElementReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/ReportReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/RootTableReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/rpt/SubDocumentReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/style/FontFaceReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/style/MasterPageReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/style/OfficeStyleReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/style/OfficeStylesReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/style/PageLayoutReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/style/StyleDefinitionReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/table/CoveredCellReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/table/TableCellReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/table/TableColumnReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/table/TableColumnsReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/table/TableReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/table/TableRowReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/table/TableRowsReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/text/NoCDATATextContentReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/text/TextContentReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/parser/xlink/XLinkReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/styles/LengthCalculator \
    reportbuilder/java/org/lofice/report/pentaho/styles/StyleMapper \
    reportbuilder/java/org/lofice/report/pentaho/styles/StyleMapperKey \
    reportbuilder/java/org/lofice/report/pentaho/styles/StyleMapperXmlFactoryModule \
    reportbuilder/java/org/lofice/report/pentaho/styles/StyleMapperXmlResourceFactory \
    reportbuilder/java/org/lofice/report/pentaho/styles/StyleMappingDocumentReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/styles/StyleMappingReadHandler \
    reportbuilder/java/org/lofice/report/pentaho/styles/StyleMappingRule \
    reportbuilder/java/org/lofice/report/util/DefaultJobProperties \
    reportbuilder/java/org/lofice/report/util/DefaultParameterMap \
    reportbuilder/java/org/lofice/report/util/DefaultReportJobDefinition \
))

$(eval $(call gb_Jar_add_packagefiles,reportbuilder,,\
	$(SRCDIR)/reportbuilder/java/jfreereport.properties \
	$(SRCDIR)/reportbuilder/java/libformula.properties \
	$(SRCDIR)/reportbuilder/java/loader.properties \
))

$(eval $(call gb_Jar_add_packagefiles,reportbuilder,org/lofice/report/function/metadata,\
	$(SRCDIR)/reportbuilder/java/org/lofice/report/function/metadata/Author-Function.properties \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/function/metadata/Author-Function_en_US.properties \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/function/metadata/Title-Function.properties \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/function/metadata/Title-Function_en_US.properties \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/function/metadata/category.properties \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/function/metadata/category_en_US.properties \
))

$(eval $(call gb_Jar_add_packagefiles,reportbuilder,org/lofice/report/pentaho,\
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/configuration.properties \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/module.properties \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/oasis-datastyle.css \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/oasis-draw.css \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/oasis-form.css \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/oasis-style.css \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/oasis-table.css \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/oasis-text.css \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/smil.css \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/star-office.css \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/star-report.css \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/star-rpt.css \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/svg.css \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/xsl-fo.css \
))

$(eval $(call gb_Jar_add_packagefiles,reportbuilder,org/lofice/report/pentaho/styles,\
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/styles/stylemapper.xml \
	$(SRCDIR)/reportbuilder/java/org/lofice/report/pentaho/styles/stylemapper.xsd \
))

# vim: set shiftwidth=4 tabstop=4 noexpandtab:
