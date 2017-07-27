#include "html_table.h"
#include "filter.h"

static const QString TableHeader = R"HTML(
<html>
<link rel="stylesheet" href="tables.css">
<table class="pure-table" id="table" style="position: absolute; top: 0; bottom: 0; left: 0; right: 0; width: 100%">
	<thead>
		<tr>
			<th>bid c</th>
			<th>theo c</th>
			<th>ask c</th>
			<th>desc</th>
			<th>bid a</th>
			<th>theo a</th>
			<th>ask a</th>
		</tr>
	</thead>)HTML";

static const QString TableFooter = "</table></html>";

QString GetTableRow(int row, const Instrument& instr)
{
	return QString("<tr") + (row % 2 ? "" : " class=\"pure-table-odd\"") + ">"
			+ "<td>" + (instr.GetBid() ? QString::number(instr.GetBid()->price) : QString("")) + "</td>"
			+ "<td>" + QString::number(instr.GetTheo()) + "</td>"
			+ "<td>" + (instr.GetAsk() ? QString::number(instr.GetAsk()->price) : QString("")) + "</td>"
			+ "<td>" + QString::fromStdString(instr.GetMarket()) + ":" + QString::fromStdString(instr.GetFeedcode()) + "</td>"
			+ "<td>" + (instr.GetBid() ? QString::number(instr.GetBid()->price) : QString("")) + "</td>"
			+ "<td>" + QString::number(instr.GetTheo()) + "</td>"
			+ "<td>" + (instr.GetAsk() ? QString::number(instr.GetAsk()->price) : QString("")) + "</td>";
}

QString HtmlRenderer::ToHtml(const std::vector<Instrument>& instruments) const
{
	QString html = TableHeader;
	int row = 0;

	for (const Instrument& instr : instruments)
	{
		if (!mFilter.Matches(instr))
			continue;

		html += GetTableRow(row, instr);
		++row;
	}

	html += TableFooter;
	return html;
}
