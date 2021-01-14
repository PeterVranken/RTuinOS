var NAVTREE =
[
  [ "RTuinOS", "index.html", [
    [ "Software Documentation for RTuinOS", "index.html", null ],
    [ "Related Pages", "pages.html", [
      [ "Todo List", "todo.html", null ]
    ] ],
    [ "Data Structures", "annotated.html", [
      [ "D_Work_integerSineZ", "struct_d___work__integer_sine_z.html", null ],
      [ "dpy_display_t", "classdpy__display__t.html", null ],
      [ "ExternalOutputs_integerSineZ", "struct_external_outputs__integer_sine_z.html", null ],
      [ "RT_MODEL_integerSineZ", "struct_r_t___m_o_d_e_l__integer_sine_z.html", null ]
    ] ],
    [ "Data Structure Index", "classes.html", null ],
    [ "Data Fields", "functions.html", null ],
    [ "File List", "files.html", [
      [ "adc_analogInput.cpp", "adc__analog_input_8cpp.html", null ],
      [ "adc_analogInput.h", "adc__analog_input_8h.html", null ],
      [ "tc12/aev_applEvents.h", "tc12_2aev__appl_events_8h.html", null ],
      [ "tc14/applCode/aev_applEvents.h", "tc14_2appl_code_2aev__appl_events_8h.html", null ],
      [ "but_button.cpp", "but__button_8cpp.html", null ],
      [ "but_button.h", "but__button_8h.html", null ],
      [ "clk_clock.cpp", "clk__clock_8cpp.html", null ],
      [ "clk_clock.h", "clk__clock_8h.html", null ],
      [ "dpy_display.cpp", "dpy__display_8cpp.html", null ],
      [ "dpy_display.h", "dpy__display_8h.html", null ],
      [ "gsl_systemLoad.c", "gsl__system_load_8c.html", null ],
      [ "gsl_systemLoad.h", "gsl__system_load_8h.html", null ],
      [ "integerSineZ.h", null, null ],
      [ "integerSineZ_private.h", null, null ],
      [ "integerSineZ_types.h", null, null ],
      [ "itq_interTaskQueue.c", "itq__inter_task_queue_8c.html", null ],
      [ "itq_interTaskQueue.h", "itq__inter_task_queue_8h.html", null ],
      [ "rtos.c", "rtos_8c.html", null ],
      [ "tc07/rtos.config.h", "tc07_2rtos_8config_8h.html", null ],
      [ "tc08/rtos.config.h", "tc08_2rtos_8config_8h.html", null ],
      [ "tc09/rtos.config.h", "tc09_2rtos_8config_8h.html", null ],
      [ "tc10/rtos.config.h", "tc10_2rtos_8config_8h.html", null ],
      [ "tc11/rtos.config.h", "tc11_2rtos_8config_8h.html", null ],
      [ "tc12/rtos.config.h", "tc12_2rtos_8config_8h.html", null ],
      [ "tc14/rtosConfig/rtos.config.h", "tc14_2rtos_config_2rtos_8config_8h.html", null ],
      [ "rtos.config.template.h", "rtos_8config_8template_8h.html", null ],
      [ "rtos.h", "rtos_8h.html", null ],
      [ "rtos_assert.h", "rtos__assert_8h.html", null ],
      [ "rtwtypes.h", null, null ],
      [ "tc12/stdout.c", "tc12_2stdout_8c.html", null ],
      [ "tc14/rtosConfig/stdout.c", "tc14_2rtos_config_2stdout_8c.html", null ],
      [ "tc12/stdout.h", "tc12_2stdout_8h.html", null ],
      [ "tc14/rtosConfig/stdout.h", "tc14_2rtos_config_2stdout_8h.html", null ],
      [ "tc07_criticalSection.c", "tc07__critical_section_8c.html", null ],
      [ "tc08_applEvents.h", "tc08__appl_events_8h.html", null ],
      [ "tc08_applInterrupt.c", "tc08__appl_interrupt_8c.html", null ],
      [ "tc09_applEvents.h", "tc09__appl_events_8h.html", null ],
      [ "tc09_pseudoMutex.c", "tc09__pseudo_mutex_8c.html", null ],
      [ "tc10_systemLoad.c", "tc10__system_load_8c.html", null ],
      [ "tc11_applEvents.h", "tc11__appl_events_8h.html", null ],
      [ "tc11_mutex.c", "tc11__mutex_8c.html", null ],
      [ "tc12_queue.c", "tc12__queue_8c.html", null ],
      [ "tc14_adcInput.cpp", "tc14__adc_input_8cpp.html", null ],
      [ "tc14_adcInput.h", "tc14__adc_input_8h.html", null ]
    ] ],
    [ "Directories", "dirs.html", [
      [ "code", "dir_4397ac5528b7614a6ff7ec237923c49d.html", [
        [ "applications", "dir_14527a9bb6522412eaac31ae93d37ae9.html", [
          [ "tc07", "dir_c3c19174c11822adef083ec2f037d1d3.html", null ],
          [ "tc08", "dir_b7bfb19fa922d05f683ffd68bba5fd8b.html", null ],
          [ "tc09", "dir_4eeb2e0e2be2977852652908d86a7995.html", null ],
          [ "tc10", "dir_3527077c643f903b43810632c752ef58.html", null ],
          [ "tc11", "dir_3a6ddc1de001da582e5832d7243826cf.html", null ],
          [ "tc12", "dir_d2352d01b3ae1a2f1f34fc6aa752f690.html", null ],
          [ "tc14", "dir_fba38348276dedd2e1ae358231e71340.html", [
            [ "applCode", "dir_7991614c2edd1e2c61912f5660fd0cd5.html", null ],
            [ "rtosConfig", "dir_06907b3b02ece29f3d2fb2cae7629c5e.html", null ]
          ] ]
        ] ],
        [ "RTOS", "dir_70589dcba19e591e3937592f828d1f9a.html", null ]
      ] ]
    ] ],
    [ "Globals", "globals.html", null ]
  ] ]
];

function createIndent(o,domNode,node,level)
{
  if (node.parentNode && node.parentNode.parentNode)
  {
    createIndent(o,domNode,node.parentNode,level+1);
  }
  var imgNode = document.createElement("img");
  if (level==0 && node.childrenData)
  {
    node.plus_img = imgNode;
    node.expandToggle = document.createElement("a");
    node.expandToggle.href = "javascript:void(0)";
    node.expandToggle.onclick = function() 
    {
      if (node.expanded) 
      {
        $(node.getChildrenUL()).slideUp("fast");
        if (node.isLast)
        {
          node.plus_img.src = node.relpath+"ftv2plastnode.png";
        }
        else
        {
          node.plus_img.src = node.relpath+"ftv2pnode.png";
        }
        node.expanded = false;
      } 
      else 
      {
        expandNode(o, node, false);
      }
    }
    node.expandToggle.appendChild(imgNode);
    domNode.appendChild(node.expandToggle);
  }
  else
  {
    domNode.appendChild(imgNode);
  }
  if (level==0)
  {
    if (node.isLast)
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2plastnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2lastnode.png";
        domNode.appendChild(imgNode);
      }
    }
    else
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2pnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2node.png";
        domNode.appendChild(imgNode);
      }
    }
  }
  else
  {
    if (node.isLast)
    {
      imgNode.src = node.relpath+"ftv2blank.png";
    }
    else
    {
      imgNode.src = node.relpath+"ftv2vertline.png";
    }
  }
  imgNode.border = "0";
}

function newNode(o, po, text, link, childrenData, lastNode)
{
  var node = new Object();
  node.children = Array();
  node.childrenData = childrenData;
  node.depth = po.depth + 1;
  node.relpath = po.relpath;
  node.isLast = lastNode;

  node.li = document.createElement("li");
  po.getChildrenUL().appendChild(node.li);
  node.parentNode = po;

  node.itemDiv = document.createElement("div");
  node.itemDiv.className = "item";

  node.labelSpan = document.createElement("span");
  node.labelSpan.className = "label";

  createIndent(o,node.itemDiv,node,0);
  node.itemDiv.appendChild(node.labelSpan);
  node.li.appendChild(node.itemDiv);

  var a = document.createElement("a");
  node.labelSpan.appendChild(a);
  node.label = document.createTextNode(text);
  a.appendChild(node.label);
  if (link) 
  {
    a.href = node.relpath+link;
  } 
  else 
  {
    if (childrenData != null) 
    {
      a.className = "nolink";
      a.href = "javascript:void(0)";
      a.onclick = node.expandToggle.onclick;
      node.expanded = false;
    }
  }

  node.childrenUL = null;
  node.getChildrenUL = function() 
  {
    if (!node.childrenUL) 
    {
      node.childrenUL = document.createElement("ul");
      node.childrenUL.className = "children_ul";
      node.childrenUL.style.display = "none";
      node.li.appendChild(node.childrenUL);
    }
    return node.childrenUL;
  };

  return node;
}

function showRoot()
{
  var headerHeight = $("#top").height();
  var footerHeight = $("#nav-path").height();
  var windowHeight = $(window).height() - headerHeight - footerHeight;
  navtree.scrollTo('#selected',0,{offset:-windowHeight/2});
}

function expandNode(o, node, imm)
{
  if (node.childrenData && !node.expanded) 
  {
    if (!node.childrenVisited) 
    {
      getNode(o, node);
    }
    if (imm)
    {
      $(node.getChildrenUL()).show();
    } 
    else 
    {
      $(node.getChildrenUL()).slideDown("fast",showRoot);
    }
    if (node.isLast)
    {
      node.plus_img.src = node.relpath+"ftv2mlastnode.png";
    }
    else
    {
      node.plus_img.src = node.relpath+"ftv2mnode.png";
    }
    node.expanded = true;
  }
}

function getNode(o, po)
{
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) 
  {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
        i==l);
  }
}

function findNavTreePage(url, data)
{
  var nodes = data;
  var result = null;
  for (var i in nodes) 
  {
    var d = nodes[i];
    if (d[1] == url) 
    {
      return new Array(i);
    }
    else if (d[2] != null) // array of children
    {
      result = findNavTreePage(url, d[2]);
      if (result != null) 
      {
        return (new Array(i).concat(result));
      }
    }
  }
  return null;
}

function initNavTree(toroot,relpath)
{
  var o = new Object();
  o.toroot = toroot;
  o.node = new Object();
  o.node.li = document.getElementById("nav-tree-contents");
  o.node.childrenData = NAVTREE;
  o.node.children = new Array();
  o.node.childrenUL = document.createElement("ul");
  o.node.getChildrenUL = function() { return o.node.childrenUL; };
  o.node.li.appendChild(o.node.childrenUL);
  o.node.depth = 0;
  o.node.relpath = relpath;

  getNode(o, o.node);

  o.breadcrumbs = findNavTreePage(toroot, NAVTREE);
  if (o.breadcrumbs == null)
  {
    o.breadcrumbs = findNavTreePage("index.html",NAVTREE);
  }
  if (o.breadcrumbs != null && o.breadcrumbs.length>0)
  {
    var p = o.node;
    for (var i in o.breadcrumbs) 
    {
      var j = o.breadcrumbs[i];
      p = p.children[j];
      expandNode(o,p,true);
    }
    p.itemDiv.className = p.itemDiv.className + " selected";
    p.itemDiv.id = "selected";
    $(window).load(showRoot);
  }
}

