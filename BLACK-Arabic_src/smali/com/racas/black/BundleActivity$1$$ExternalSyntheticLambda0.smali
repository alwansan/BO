.class public final synthetic Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda0;
.super Ljava/lang/Object;
.source "D8$$SyntheticClass"

# interfaces
.implements Lcom/racas/black/BundleFiles$Source;


# instance fields
.field public final synthetic f$0:Lcom/racas/black/BundleActivity$1;


# direct methods
.method public synthetic constructor <init>(Lcom/racas/black/BundleActivity$1;)V
    .locals 0

    .line 0
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    iput-object p1, p0, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda0;->f$0:Lcom/racas/black/BundleActivity$1;

    return-void
.end method


# virtual methods
.method public final open()Ljava/io/InputStream;
    .locals 1

    .line 0
    iget-object v0, p0, Lcom/racas/black/BundleActivity$1$$ExternalSyntheticLambda0;->f$0:Lcom/racas/black/BundleActivity$1;

    invoke-virtual {v0}, Lcom/racas/black/BundleActivity$1;->lambda$run$0$com-racas-black-BundleActivity$1()Ljava/io/InputStream;

    move-result-object v0

    return-object v0
.end method
